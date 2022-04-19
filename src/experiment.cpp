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

#define MIRRORSHAPE 2
#define DIM 9

using namespace std;

int main(int argc, char *argv[]) {

  LOG("Start");
  bool noopt = false;
  bool norand = false;
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

  ArgParser args(
      "-noopt -norand --prefix --optrays --optsegments --rays --segments", argc,
      argv);
  args.getFlag("-noopt", noopt);
  args.getFlag("-norand", norand);
  args.getArg("--optrays", optrays);
  args.getArg("--optsegments", optsegments);
  args.getArg("--rays", rays);
  args.getArg("--segments", segments);
  args.getArg("--prefix", prefix);

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
  auto referenceReflector =
      csvReader.read<double, double>("../data/reference_reflector.csv");

  auto absorptionSpectrum = getFunction(absorptionData);
  auto emissionSpectrum = getFunction(emissionData);

  LOG("Read Data");

  vecn<double, DIM> params;
  vecn<double, DIM> paramsStart;
  vecn<double, DIM> paramsLower;
  vecn<double, DIM> paramsUpper;

  double irradianceCrystal = 0.0;

#if (MIRRORSHAPE == 0)
  auto mirrorShape = [&](double x) {
    dvec2 point = {x, params[0] * x * x + params[1]};
    return point;
  };
  paramsStart = {0.0, 0.0};
  paramsLower = {0.0, 0.0};
  paramsUpper = {1.0, 1.0};
#elif (MIRRORSHAPE == 1)
  auto mirrorShape = [&](double x) {
    dvec2 start = {0.0042, 0.0};
    dvec2 paramPoint1 = {params[0], params[1]};
    dvec2 paramPoint2 = {params[2], params[3]};
    dvec2 end = {0.15, 0.224};
    dvec2 point = bezier(start, paramPoint1, paramPoint2, end, x);
    return point;
  };
  paramsStart = {0.0, 0.0, 0.075, 0.0};
  paramsLower = {0.0, 0.0, 0.075, 0.0};
  paramsUpper = {0.075, 0.3, 0.15, 0.3};
#elif (MIRRORSHAPE == 2)
  auto mirrorShape = [&](double x) {
    dvec2 start = {params[4], params[5]};
    dvec2 paramPoint1 = {params[0], params[1]};
    dvec2 paramPoint2 = {params[2], params[3]};
    dvec2 end = {params[6], params[7]};
    dvec2 point = bezier(start, paramPoint1, paramPoint2, end, x);
    return point;
  };
  paramsStart = {0.0042,  0.0, 0.0042,  0.0, 0.0042,  0.0, 0.6000,  0.1,  0.0};
  paramsLower = {0.0042, -2.0, 0.0042, -2.0, 0.0042, -2.0, 0.0042, -2.0, -0.3};
  paramsUpper = {1.0000,  2.0, 1.0000,  2.0, 1.0000,  2.0, 1.0000,  2.0,  2.0};

  auto monotonyConstraint = [](const vecn<double, DIM> &currentParams) {
    if ((currentParams[4] <= currentParams[0] &&
         currentParams[0] <= currentParams[2] &&
         currentParams[2] <= currentParams[6]) &&
        (currentParams[5] <= currentParams[1] &&
         currentParams[1] <= currentParams[3] &&
         currentParams[3] <= currentParams[7]))
      return -1.0;
    return 1.0;
  };

  vector<function<double(const vecn<double, DIM> &)>> constraints = {
      monotonyConstraint};

#endif

  dvec2 mirrorPos = {0.122, 0.0};
  dvec2 crystalPos = {0.0475, 0.0};

  auto optmirror = make_shared<Mirror2D>(
      Mirror2D(mirrorPos, {-1.0, 0.0}, 4, mirrorShape, optsegments));

  auto optcrystal = make_shared<Grid2D>(Grid2D(
      crystalPos, {-0.0475, -0.003}, {0.0475, 0.003}, 158, 10, sellmeierNdYag,
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

  auto trace = [&](const vecn<double, DIM> &currentParams) {
    params = currentParams;
    optcrystal->setPos(crystalPos + dvec2(params[8], 0.0));
    optmirror->setPos(mirrorPos + dvec2(params[8], 0.0));
    optscene.init();
    optraysStorage = optscene.trace(4);
    double functional = -optcrystal->sum();
    optcrystal->reset();
    irradianceCrystal = 0.0;
    return functional;
  };

  auto traceVar = [&](const vecn<double, DIM> &currentParams) {
    params = currentParams;
    optcrystal->setPos(crystalPos + dvec2(params[8], 0.0));
    optmirror->setPos(mirrorPos + dvec2(params[8], 0.0));
    optscene.init();
    optraysStorage = optscene.trace(4);
    double functional = optcrystal->var();
    optcrystal->reset();
    irradianceCrystal = 0.0;
    return functional;
  };

  vector<vecn<double, DIM>> solutionParams;

  // Optimization Algorithm
  if (!noopt) {
    // initially sarch randomly for highest power
    if (!norand) {
      cout << "Initial search: " << endl;
      UniformSamplerND<DIM> sampler;
      sampler.init(1000000);
      double optPower = numeric_limits<double>::max();
      size_t bb_eval = 0;
      size_t max_bb_eval = 100000;
      for (size_t i = 0; i < 1000000; ++i) {
        auto sample = sampler.next();
        vecn<double, DIM> searchPoint;
        for (size_t j = 0; j < DIM; ++j) {
          searchPoint[j] =
              paramsLower[j] + sample[j] * (paramsUpper[j] - paramsLower[j]);
        }
        for (auto constraint : constraints) {
          if (constraint(searchPoint) > 0.0) {
            continue;
          }
        }
        if (bb_eval > max_bb_eval)
          break;
        double power = trace(searchPoint);
        bb_eval++;
        if (power < optPower) {
          optPower = power;
          paramsStart = searchPoint;
          cout << "(" << bb_eval << "/" << max_bb_eval << ") "
               << "Better starting point: " << optPower
               << " params: " << paramsStart << endl;
        }
      }
      LOG("Initial search");
    }

    cout << "Optimization: " << endl;
    auto solutions = mads<DIM>(trace, traceVar, paramsStart, paramsLower,
                               paramsUpper, constraints);

    if (solutions.empty()) {
      DEBUG("Error: No solutions found");
      return -1;
    }

    LOG("Optimization");

    cout << "Solutions:" << endl;
    cout << endl;

    for (size_t i = 0; i < solutions.size(); ++i) {
      auto [params, power, variance] = solutions[i];
      cout << "[" << i << "] " << -power << " " << variance << endl;
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
      Mirror2D(mirrorPos, {-1.0, 0.0}, 4, mirrorShape, segments));

  auto crystal = make_shared<Grid2D>(Grid2D(
      crystalPos, {-0.0475, -0.003}, {0.0475, 0.003}, 158, 10, sellmeierNdYag,
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
  for (size_t i = 0; i < solutionParams.size(); ++i) {
    cout << endl;
    cout << "Solution " << i << endl;
    params = solutionParams[i];
    crystal->setPos(crystalPos + dvec2(params[8], 0.0));
    mirror->setPos(mirrorPos + dvec2(params[8], 0.0));
    scene.init();
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

    CSVWriter csvWriter("out/" + prefix + "/solution" + to_string(i), ".txt");

    auto startPoint = mirrorShape(0.0);
    auto endPoint = mirrorShape(1.0);

    double asldMirrorZcoordinate =
        ((mirror->getPos().x - (crystal->getPos().x - 0.0475)) -
         (startPoint.y + (endPoint.y - startPoint.y) / 2.0)) *
        1000.0;

    double asldLensZcoordinate =
        (lens->getPos().x - (crystal->getPos().x - 0.0475)) * 1000.0;

    csvWriter.add("reflector", "#asldMirrorZcoordinate", asldMirrorZcoordinate);
    csvWriter.add("reflector", "#asldLensZcoordinate", asldLensZcoordinate);

    for (size_t j = 0; j <= 1000; ++j) {
      dvec2 point = mirrorShape(double(1000 - j) / 1000);
      double xScaled = (-point.y * 1000.0);
      double yScaled = point.x * 1000.0;
      csvWriter.add("reflector", xScaled, yScaled);
    }
    csvWriter.write();

    VTKWriter vtkWriter("out/" + prefix + "/solution" + to_string(i));
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
