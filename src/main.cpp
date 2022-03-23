#include "argparser.h"
#include "csv.h"
#include "debugutils.h"
#include "functionutils.h"
#include "grid.h"
#include "lens.h"
#include "log.h"
#include "mirror.h"
#include "nomadbinding3.h"
#include "optimization.h"
#include "random.h"
#include "ray.h"
#include "scene.h"
#include "shape.h"
#include "vecn.h"
#include "vtk.h"
#include <cmath>
#include <cstddef>

using namespace std;

int main(int argc, char *argv[]) {

  LOG("Start");
  bool noopt = false;
  size_t optrays = 10000;
  size_t optsegments = 100;
  size_t rays = 100000;
  size_t segments = 10000;

  ArgParser args("-noopt --optrays --optsegments --rays --segments", argc, argv);
  args.getFlag("-noopt", noopt);
  args.getArg("--optrays", optrays);
  args.getArg("--optsegments", optsegments);
  args.getArg("--rays", rays);
  args.getArg("--segments", segments);

  cout << "noopt: " << noopt << endl;
  cout << "optrays: " << optrays << endl;
  cout << "optsegments: " << optsegments << endl;
  cout << "rays: " << rays << endl;
  cout << "segments: " << segments << endl;

  LOG("Parse Arguments");

  CSVReader csvReader;
  auto absorptionData =
      csvReader.read<double, double>("../data/ndyag_absorption_spectrum.csv");
  auto emissionData =
      csvReader.read<double, double>("../data/solar_spectrum.csv");

  auto absorptionSpectrum = getFunction(absorptionData);
  auto emissionSpectrum = getFunction(emissionData);

  LOG("Read Data");
  double solarConstant = 1361.0; // W*m^-2
  double emittorRadius = 0.6;    // m
  double solarPower = solarConstant * M_PI * emittorRadius * emittorRadius;

  vecn<double, 4> params;
  double irradianceCrystal = 0.0;

  auto mirrorShapeParabola = [&](double x) {
    dvec2 point = {x, params[0] * x * x + params[1]};
    return point;
  };

  auto mirrorShapeBezier = [&](double x) {
    dvec2 start = {0.006, 0.0};
    dvec2 paramPoint1 = {params[0], params[1]};
    dvec2 paramPoint2 = {params[2], params[3]};
    dvec2 end = {0.15, 0.3};
    dvec2 point = bezier(start, paramPoint1, paramPoint2, end, x);
    return point;
  };

  auto sellmeierNdYag = [](double wavelength) {
    double wavelengthMu = wavelength / 1000.;
    double wavelengthSqrd = wavelengthMu * wavelengthMu;
    double nSqrd = (2.282 * wavelengthSqrd) / (wavelengthSqrd - 0.01185) +
                  (3.27644 * wavelengthSqrd) / (wavelengthSqrd - 282.734) +
                  1.0;
    return sqrt(nSqrd);
  };

  auto optmirror = make_shared<Mirror2D>(
      Mirror2D({1.800, 0.0}, {-1.0, 0.0}, mirrorShapeBezier, optsegments));

  auto optcrystal = make_shared<Grid2D>(Grid2D(
      {1.7425, 0.0}, {-0.0475, -0.003}, {0.0475, 0.003}, 158, 10,
      [&](Ray2D &ray, double distance, double &cell) {
        // Lambert law of absorption
        double alpha = absorptionSpectrum(ray.wavelength);
        double remainingPower = ray.power * exp(-alpha * distance * 100.0);
        double absorbedPower = ray.power - remainingPower;
        cell += absorbedPower;
        ray.power = remainingPower;
      },
      [&](Ray2D &ray, const IntersectResult2D &result) {
        irradianceCrystal += ray.power;
      },
      sellmeierNdYag));

  auto optlens =
      make_shared<Lens2D>(Lens2D({0.0, 0.0}, {1.0, 0.0}, 0.6, 1.2));

  Scene2D optscene;

  optscene.add(optmirror);
  optscene.add(optcrystal);
  optscene.add(optlens);

  RNG::StratifiedSampler1D originSampler;
  RNG::ImportanceSampler1D absorptionImpSampler(absorptionSpectrum, 300.0,
                                                1000.0);

  optscene.generateDirectionalRays({-2.0, 0.0}, emittorRadius, {1.0, 0.0},
                                solarPower, optrays, originSampler,
                                absorptionImpSampler, emissionSpectrum);

  LOG("Preprocessing");

  vector<vector<Ray2D>> optraysStorage;

  auto trace = [&](const vecn<double, 4> &currentParams) {
    params = currentParams;
    optmirror->rebuild();
    optraysStorage = optscene.trace(4);
    double functional = -optcrystal->sum();
    optcrystal->reset();
    irradianceCrystal = 0.0;
    return functional;
  };

  auto traceVar = [&](const vecn<double, 4> &currentParams) {
    params = currentParams;
    optmirror->rebuild();
    optraysStorage = optscene.trace(4);
    double functional = optcrystal->var();
    optcrystal->reset();
    irradianceCrystal = 0.0;
    return functional;
  };

  params[0] = 0.07281232625;
  params[1] = 0.002341829939;
  params[2] = 0.07726241648;
  params[3] = 0.02685499005;

  // Optimization Algorithm
  if (!noopt) {
    cout << "Mirror Optimizer: " << endl;

    auto solutions =
        mads<4>(trace, traceVar, {0.0, 0.0, 0.075, 0.0},
                {0.0, 0.0, 0.075, 0.0}, {0.075, 0.3, 0.15, 0.3});
    if (solutions.empty()) {
      DEBUG("Error: No solutions found");
      return -1;
    }

    auto minimizingParameters = solutions[0];
    params = minimizingParameters;
  }

  auto mirror = make_shared<Mirror2D>(
      Mirror2D({1.800, 0.0}, {-1.0, 0.0}, mirrorShapeBezier, segments));

  auto crystal = make_shared<Grid2D>(Grid2D(
      {1.7425, 0.0}, {-0.0475, -0.003}, {0.0475, 0.003}, 158, 10,
      [&](Ray2D &ray, double distance, double &cell) {
        // Lambert law of absorption
        double alpha = absorptionSpectrum(ray.wavelength);
        double remainingPower = ray.power * exp(-alpha * distance * 100.0);
        double absorbedPower = ray.power - remainingPower;
        cell += absorbedPower;
        ray.power = remainingPower;
      },
      [&](Ray2D &ray, const IntersectResult2D &result) {
        irradianceCrystal += ray.power;
      },
      sellmeierNdYag));

  auto lens =
      make_shared<Lens2D>(Lens2D({0.0, 0.0}, {1.0, 0.0}, 0.6, 1.2));

  Scene2D scene;

  scene.add(mirror);
  scene.add(crystal);
  scene.add(lens);

  scene.generateDirectionalRays({-2.0, 0.0}, emittorRadius, {1.0, 0.0},
                                solarPower, rays, originSampler,
                                absorptionImpSampler, emissionSpectrum);
  
  vector<vector<Ray2D>> raysStorage;
  mirror->rebuild();
  raysStorage = scene.trace(4);

  double irradiationEfficiency = irradianceCrystal / solarPower;
  double absorbedPower = crystal->sum();
  double absorbedPowerVar = crystal->var();
  double absorptionEfficiencyTotal = absorbedPower / solarPower;
  double absorptionEfficiencyIrradiated = absorbedPower / irradianceCrystal;

  LOG("Tracing");

  cout << "Final parameters: " << params << endl;
  cout << "Solar power: " << solarPower << endl;
  cout << "Irradiance crystal: " << irradianceCrystal << endl;
  cout << "Irradiation efficiency: " << irradiationEfficiency << endl;
  cout << "Absorbed power: " << absorbedPower << endl;
  cout << "Absorbed power (var): " << absorbedPowerVar << endl;
  cout << "Absorption efficiency (total): " << absorptionEfficiencyTotal << endl;
  cout << "Absorption efficiency (irradiated): " << absorptionEfficiencyIrradiated << endl;

  VTKWriter vtkWriter("vtkOut");
  vtkWriter.add(mirror, "mirror");
  vtkWriter.add(mirror->getAABBs(), "mirror.AABB");
  vtkWriter.add(crystal, "crystal");
  vtkWriter.add(crystal->getAABBs(), "crystal.AABB");
  vtkWriter.add(lens, "lens");
  vtkWriter.add(lens->getAABBs(), "lens.AABB");
  vtkWriter.addAsSequence(raysStorage, "rays", 100);
  vtkWriter.addAsComposition(raysStorage, "rays_composition", 100);
  vtkWriter.write();

  LOG("Output");
}
