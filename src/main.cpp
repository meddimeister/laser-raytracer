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
  bool noopt = false;
  size_t optrays = 10000;
  size_t optsegments = 100;
  size_t rays = 100000;
  size_t segments = 10000;
  bool samplertest = false;
  string prefix = "";

  double solarConstant = 1361.0; // W*m^-2
  double emittorRadius = 0.6;    // m
  double solarPower = solarConstant * M_PI * emittorRadius * emittorRadius;
  double solarDivergence = 0.53338 * (2.0 * M_PI / 360.0); // rad
  vecn<double, 4> sellmeierNdYag = {2.282, 0.01185, 3.27644, 282.734};

  vecn<double, 4> params;
  double irradianceCrystal = 0.0;

  params[0] = 0.07281232625;
  params[1] = 0.002341829939;
  params[2] = 0.07726241648;
  params[3] = 0.02685499005;

  ArgParser args(
      "-noopt -samplertest --prefix --optrays --optsegments --rays --segments --param0 "
      "--param1 --param2 --param3",
      argc, argv);
  args.getFlag("-noopt", noopt);
  args.getFlag("-samplertest", samplertest);
  args.getArg("--optrays", optrays);
  args.getArg("--optsegments", optsegments);
  args.getArg("--rays", rays);
  args.getArg("--segments", segments);
  args.getArg("--param0", params[0]);
  args.getArg("--param1", params[1]);
  args.getArg("--param2", params[2]);
  args.getArg("--param3", params[3]);
  args.getArg("--prefix", prefix);

  cout << "noopt: " << noopt << endl;
  cout << "samplertest: " << samplertest << endl;
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
  auto referenceReflector =
      csvReader.read<double, double>("../data/reference_reflector.csv");

  auto absorptionSpectrum = getFunction(absorptionData);
  auto emissionSpectrum = getFunction(emissionData);

  LOG("Read Data");

  if (samplertest) {
    UniformSamplerND<2> uniformSampler2D;
    NormalSamplerND<2> normalSampler2D;
    UniformBallSampler<3> uniformBallSampler;

    uniformSampler2D.init(1000);
    normalSampler2D.init(1000);
    uniformBallSampler.init(1000);

    CSVWriter csvWriter("csvOut/samplertest");

    for (size_t i = 0; i < 1000; ++i) {
      auto uniformSample2D = uniformSampler2D.next();
      auto normalSample2D = normalSampler2D.next();
      auto uniformBallSample = uniformBallSampler.next();
      csvWriter.add("uniformSampler2D", uniformSample2D[0], uniformSample2D[1]);
      csvWriter.add("normalSampler2D", normalSample2D[0], normalSample2D[1]);
      csvWriter.add("uniformBallSampler", uniformBallSample[0],
                    uniformBallSample[1], uniformBallSample[2]);
    }
    csvWriter.write();

    LOG("Sampler Test");
    return 0;
  }

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

  auto optmirror = make_shared<Mirror2D>(
      Mirror2D({0.122, 0.0}, {-1.0, 0.0}, 4, mirrorShapeBezier, optsegments));

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

  auto trace = [&](const vecn<double, 4> &currentParams) {
    params = currentParams;
    optmirror->init();
    optraysStorage = optscene.trace(4);
    double functional = -optcrystal->sum();
    optcrystal->reset();
    irradianceCrystal = 0.0;
    return functional;
  };

  auto traceVar = [&](const vecn<double, 4> &currentParams) {
    params = currentParams;
    optmirror->init();
    optraysStorage = optscene.trace(4);
    double functional = optcrystal->var();
    optcrystal->reset();
    irradianceCrystal = 0.0;
    return functional;
  };

  vector<vecn<double, 4>> solutionParams;

  // Optimization Algorithm
  if (!noopt) {
    cout << "Mirror Optimizer: " << endl;

    auto solutions = mads<4>(trace, traceVar, {0.0, 0.0, 0.075, 0.0},
                             {0.0, 0.0, 0.075, 0.0}, {0.075, 0.3, 0.15, 0.3});
    if (solutions.empty()) {
      DEBUG("Error: No solutions found");
      return -1;
    }

    LOG("Optimizer");

    cout << "Solutions:" << endl;
    cout << endl;

    for (size_t i = 0; i < solutions.size(); ++i) {
      auto [params, power, variance] = solutions[i];
      cout << "[" << i << "] "
           << -power << " " << variance << endl;
    }

    cout << endl;
    cout << "Choose solutions to output: " << endl;

    string solutionsString;
    getline(cin, solutionsString);
    stringstream ss(solutionsString);
    size_t idx;
    while (ss >> idx) {
      solutionParams.push_back(get<0>(solutions[idx]));
    }
  } else {
    solutionParams.push_back(params);
  }

  auto mirror = make_shared<Mirror2D>(
      Mirror2D({0.122, 0.0}, {-1.0, 0.0}, 4, mirrorShapeBezier, segments));

  auto crystal = make_shared<Grid2D>(Grid2D(
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

  auto lens =
      make_shared<ThinLens2D>(ThinLens2D({-1.585, 0.0}, {1.0, 0.0}, 0.7, 1.2));

  Scene2D scene;

  scene.add(mirror);
  scene.add(crystal);
  scene.add(lens);

  scene.generateDirectionalRays({-1.590, 0.0}, emittorRadius, {1.0, 0.0},
                                solarDivergence, solarPower, rays,
                                originSampler, divergenceSampler,
                                absorptionImpSampler, emissionSpectrum);

  vector<vector<Ray2D>> raysStorage;
  for (size_t i = 0; i <  solutionParams.size(); ++i) {
    cout << endl;
    cout << "Solution " << i << endl;
    params = solutionParams[i];
    mirror->init();
    raysStorage = scene.trace(4);

    double irradiationEfficiency = irradianceCrystal / solarPower;
    double absorbedPower = crystal->sum();
    double absorbedPowerVar = crystal->var();
    double absorptionEfficiencyTotal = absorbedPower / solarPower;
    double absorptionEfficiencyIrradiated = absorbedPower / irradianceCrystal;

    cout << "Parameters: " << params << endl;
    cout << "Solar power: " << solarPower << endl;
    cout << "Irradiance crystal: " << irradianceCrystal << endl;
    cout << "Irradiation efficiency: " << irradiationEfficiency << endl;
    cout << "Absorbed power: " << absorbedPower << endl;
    cout << "Absorbed power (var): " << absorbedPowerVar << endl;
    cout << "Absorption efficiency (total): " << absorptionEfficiencyTotal
         << endl;
    cout << "Absorption efficiency (irradiated): "
         << absorptionEfficiencyIrradiated << endl;

    vector<tuple<double, double>> points;
    for (size_t i = 0; i <= segments; ++i) {
      dvec2 point = mirrorShapeBezier(double(i) / segments);
      points.push_back({-point.y + mirrorShapeBezier(1.0).x, point.x});
    }
    auto mirrorXYFunction = getFunction(points, true);
    //outputFunction(mirrorXYFunction, 0.0, mirrorShapeBezier(0.0).y, "csvOut", "test");

    CSVWriter csvWriter("out/"+prefix+"/solution"+to_string(i), ".txt");
    for (size_t j = 0; j <= 1000; ++j) {
      dvec2 point = mirrorShapeBezier(double(1000-j)/1000);
      double xScaled = (-point.y * 1000.0) + 224.0 + 300.2396507;
      double yScaled = point.x * 1000.0;
      csvWriter.add("reflector", xScaled, yScaled);
    }
    csvWriter.write();

    VTKWriter vtkWriter("out/"+prefix+"/solution"+to_string(i));
    vtkWriter.add(mirror, "mirror");
    vtkWriter.add(mirror->getAABBs(), "mirror.AABB");
    vtkWriter.add(crystal, "crystal");
    vtkWriter.add(crystal->getAABBs(), "crystal.AABB");
    vtkWriter.add(lens, "lens");
    vtkWriter.add(lens->getAABBs(), "lens.AABB");
    vtkWriter.addAsSequence(raysStorage, "rays", 0.01);
    vtkWriter.addAsComposition(raysStorage, "rays_composition", 0.01);
    vtkWriter.write();

    crystal->reset();
    irradianceCrystal = 0.0;
  }

  LOG("Output");
}
