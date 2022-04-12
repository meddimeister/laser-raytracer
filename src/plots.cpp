#include "math/sampler.h"
#include "optimization/optimization.h"
#include "tracing/grid.h"
#include "tracing/medium.h"
#include "tracing/mirror.h"
#include "tracing/ray.h"
#include "tracing/scene.h"
#include "tracing/shape.h"
#include "tracing/thinlens.h"
#include "types/vecn.h"
#include "utils/argparser.h"
#include "utils/csv.h"
#include "utils/debugutils.h"
#include "utils/functionutils.h"
#include "utils/log.h"
#include "utils/vtk.h"
#include <cmath>
#include <cstddef>

using namespace std;

int main(int argc, char *argv[]) {

  LOG("Start");
  size_t optrays = 10000;
  size_t optsegments = 100;

  double solarConstant = 1361.0; // W*m^-2
  double emittorRadius = 0.6;    // m
  double solarPower = solarConstant * M_PI * emittorRadius * emittorRadius;
  double solarDivergence = 0.53338 * (2.0 * M_PI / 360.0); // rad
  vecn<double, 4> sellmeierNdYag = {2.282, 0.01185, 3.27644, 282.734};

  vecn<double, 2> params;
  double irradianceCrystal = 0.0;

  params[0] = 0.07281232625;
  params[1] = 0.002341829939;

  ArgParser args(
      "--optrays --optsegments --param0 "
      "--param1",
      argc, argv);
  args.getArg("--optrays", optrays);
  args.getArg("--optsegments", optsegments);
  args.getArg("--param0", params[0]);
  args.getArg("--param1", params[1]);

  cout << "optrays: " << optrays << endl;
  cout << "optsegments: " << optsegments << endl;

  LOG("Parse Arguments");

  CSVReader csvReader;
  auto absorptionData =
      csvReader.read<double, double>("../data/ndyag_absorption_spectrum.csv");
  auto emissionData =
      csvReader.read<double, double>("../data/solar_spectrum.csv");
  auto referenceReflector =
      csvReader.read<double, double>("../data/reference_reflector.csv");

  auto absorptionSpectrum = getFunction(absorptionData);
  auto emissionSpectrum = getFunction(emissionData);

  LOG("Read Data");

  auto mirrorShapeParabola = [&](double x) {
    dvec2 point = {x, params[0] * x * x + params[1]};
    return point;
  };

  auto mirrorShapeBezier = [&](double x) {
    dvec2 start = {0.0042, 0.0};
    dvec2 paramPoint1 = {params[0], params[1]};
    dvec2 paramPoint2 = {params[2], params[3]};
    dvec2 end = {0.15, 0.224};
    dvec2 point = bezier(start, paramPoint1, paramPoint2, end, x);
    return point;
  };

  auto mirrorShapeBezierOpen = [&](double x) {
    dvec2 start = {0.0042, params[4]};
    dvec2 paramPoint1 = {params[0], params[1]};
    dvec2 paramPoint2 = {params[2], params[3]};
    dvec2 end = {0.15, params[5]};
    dvec2 point = bezier(start, paramPoint1, paramPoint2, end, x);
    return point;
  };

  auto optmirror = make_shared<Mirror2D>(
      Mirror2D({0.122, 0.0}, {-1.0, 0.0}, 4, mirrorShapeParabola, optsegments));

  auto optcrystal = make_shared<Grid2D>(Grid2D(
      {0.0475, 0.0}, {-0.0475, -0.003}, {0.0475, 0.003}, 158, 10,
      sellmeierNdYag,
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
      }));

  auto optlens =
      make_shared<ThinLens2D>(ThinLens2D({-1.585, 0.0}, {1.0, 0.0}, 0.7, 1.2));

  Scene2D optscene;

  optscene.add(optmirror);
  optscene.add(optcrystal);
  optscene.add(optlens);

  StratifiedSampler1D originSampler;
  UniformSampler1D divergenceSampler;
  ImportanceSampler1D absorptionImpSampler(absorptionSpectrum, 300.0, 1000.0);

  optscene.generateDirectionalRays({-1.590, 0.0}, emittorRadius, {1.0, 0.0},
                                   solarDivergence, solarPower, optrays,
                                   originSampler, divergenceSampler,
                                   absorptionImpSampler, emissionSpectrum);

  LOG("Preprocessing");

  vector<vector<Ray2D>> optraysStorage;

  auto trace = [&](const vecn<double, 2> &currentParams) {
    params = currentParams;
    optmirror->init();
    optraysStorage = optscene.trace(4);
    double functional = -optcrystal->sum();
    optcrystal->reset();
    irradianceCrystal = 0.0;
    return functional;
  };

  auto traceVar = [&](const vecn<double, 2> &currentParams) {
    params = currentParams;
    optmirror->init();
    optraysStorage = optscene.trace(4);
    double functional = optcrystal->var();
    optcrystal->reset();
    irradianceCrystal = 0.0;
    return functional;
  };

  UniformSamplerND<2> pointSampler;
  pointSampler.init(10000);

  dvec2 yRange = {-0.05, 0.05};
  dvec2 xRange = {0.45, 0.55};

  CSVWriter csvWriter("csvOut/plots");
  for (size_t i = 0; i <= 10000; ++i) {
    auto sample = pointSampler.next();
    vecn<double, 2> point = {xRange.x + sample[0] * (xRange.y - xRange.x),
                             yRange.x + sample[1] * (yRange.y - yRange.x)};
    csvWriter.add("power", point[0], point[1], trace(point));
    if(i%100 == 0)
      cout << (double(i)/10000) *100.0 << " %" << endl; 
  }
  csvWriter.write();

  LOG("Output");
}
