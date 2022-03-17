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

  bool noopt = false;

  ArgParser args("-noopt", argc, argv);
  args.getFlag("-noopt", noopt);

  LOG("Start");
  CSVReader csvReader;
  auto absorptionData =
      csvReader.read<float, float>("../data/ndyag_absorption_spectrum.csv");
  auto emissionData =
      csvReader.read<float, float>("../data/solar_spectrum.csv");

  auto absorptionSpectrum = getFunction(absorptionData);
  auto emissionSpectrum = getFunction(emissionData);

  LOG("Read Data");
  float solarConstant = 1361.0f; // W*m^-2
  float emittorRadius = 0.6f;    // m
  float solarPower = solarConstant * M_PI * emittorRadius * emittorRadius;

  vecn<float, 4> params;
  float irradianceCrystal = 0.0f;

  auto mirrorShapeParabola = [&](float x) {
    vec2 point = {x, params[0] * x * x + params[1]};
    return point;
  };

  auto mirrorShapeBezier = [&](float x) {
    vec2 start = {0.006f, 0.0f};
    vec2 paramPoint1 = {params[0], params[1]};
    vec2 paramPoint2 = {params[2], params[3]};
    vec2 end = {0.15f, 0.3f};
    vec2 point = bezier(start, paramPoint1, paramPoint2, end, x);
    return point;
  };

  auto sellmeierNdYag = [](float wavelength) {
    float wavelengthMu = wavelength / 1000.f;
    float wavelengthSqrd = wavelengthMu * wavelengthMu;
    float nSqrd = (2.282f * wavelengthSqrd) / (wavelengthSqrd - 0.01185f) +
                  (3.27644f * wavelengthSqrd) / (wavelengthSqrd - 282.734f) +
                  1.0f;
    return sqrt(nSqrd);
  };

  auto mirror = make_shared<Mirror2D>(
      Mirror2D({1.800f, 0.0f}, {-1.0f, 0.0f}, mirrorShapeBezier, 100));

  auto crystal = make_shared<Grid2D>(Grid2D(
      {1.7425f, 0.0f}, {-0.0475f, -0.003f}, {0.0475f, 0.003f}, 158, 10,
      [&](Ray2D &ray, float distance, float &cell) {
        // Lambert law of absorption
        float alpha = absorptionSpectrum(ray.wavelength);
        float remainingPower = ray.power * exp(-alpha * distance * 100.0f);
        float absorbedPower = ray.power - remainingPower;
        cell += absorbedPower;
        ray.power = remainingPower;
      },
      [&](Ray2D &ray, const IntersectResult2D &result) {
        irradianceCrystal += ray.power;
      },
      sellmeierNdYag));

  auto lens =
      make_shared<Lens2D>(Lens2D({0.0f, 0.0f}, {1.0f, 0.0f}, 0.6f, 1.2f));

  Scene2D scene;

  scene.add(mirror);
  scene.add(crystal);
  scene.add(lens);

  RNG::StratifiedSampler1D originSampler;
  RNG::ImportanceSampler1D absorptionImpSampler(absorptionSpectrum, 300.0f,
                                                1000.0f);

  scene.generateDirectionalRays({-2.0f, 0.0f}, emittorRadius, {1.0f, 0.0f},
                                solarPower, 10000, originSampler,
                                absorptionImpSampler, emissionSpectrum);

  LOG("Preprocessing");

  vector<vector<Ray2D>> rays;

  auto trace = [&](const vecn<float, 4> &currentParams) {
    params = currentParams;
    mirror->rebuild();
    rays = scene.trace(4);
    float functional = -crystal->sum();
    crystal->reset();
    irradianceCrystal = 0.0f;
    return functional;
  };

  auto traceVar = [&](const vecn<float, 4> &currentParams) {
    params = currentParams;
    mirror->rebuild();
    rays = scene.trace(4);
    float functional = crystal->var();
    crystal->reset();
    irradianceCrystal = 0.0f;
    return functional;
  };

  params[0] = 0.07281232625f;
  params[1] = 0.002341829939f;
  params[2] = 0.07726241648f;
  params[3] = 0.02685499005f;

  // Optimization Algorithm
  if (!noopt) {
    cout << "Mirror Optimizer: " << endl;

    auto solutions =
        mads<4>(trace, traceVar, {0.0f, 0.0f, 0.075f, 0.0f},
                {0.0f, 0.0f, 0.075f, 0.0f}, {0.075f, 0.3f, 0.15f, 0.3f});
    if (solutions.empty()) {
      DEBUG("Error: No solutions found");
      return -1;
    }

    auto minimizingParameters = solutions[0];
    params = minimizingParameters;
  }

  mirror->rebuild();
  rays = scene.trace(4);
  float irradiationEfficiency = irradianceCrystal / solarPower;
  float absorbedPower = crystal->sum();
  float absorbedPowerVar = crystal->var();
  float absorptionEfficiencyTotal = absorbedPower / solarPower;
  float absorptionEfficiencyIrradiated = absorbedPower / irradianceCrystal;

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
  vtkWriter.addAsSequence(rays, "rays", 100);
  vtkWriter.addAsComposition(rays, "rays_composition", 100);
  vtkWriter.write();

  LOG("Output");
}
