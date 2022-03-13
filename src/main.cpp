#include "csv.h"
#include "debugutils.h"
#include "functionutils.h"
#include "grid.h"
#include "gtx/spline.hpp"
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
  LOG("Read Data");
  CSVReader csvReader;
  auto absorptionData =
      csvReader.read<float, float>("../data/ndyag_absorption_spectrum.csv");
  auto emissionData =
      csvReader.read<float, float>("../data/solar_spectrum.csv");

  auto absorptionSpectrum = getFunction(absorptionData);
  auto emissionSpectrum = getFunction(emissionData);

  CSVWriter csvWriter("csvOut");

  // LOG("Test");
  // outputHistogramm(impSampler, "csvOut", "absorptionHistogramm");

  // csvWriter.add("emissionSamples", "#sample", "power");
  // vector<tuple<float, float>> values;
  // for(size_t i = 0; i < 10000; ++i){
  //   float impSample = absorptionImpSampler.next();
  //   float impPdf = absorptionImpSampler.pdf(impSample);

  //  float power = emissionPdf(impSample)/impPdf;
  //  if(isinf(power)){
  //    continue;
  //  }
  //  values.emplace_back(impSample, power);
  //  csvWriter.add("emissionSamples", impSample, power);
  //}

  // auto emissionCdf = getCdfFunction(emissionPdf, 0.0f, 1.0f);
  // auto emissionSampleFunction = getFunction(values);
  // auto emissionSamplePdf = getPdfFunction(emissionSampleFunction,
  // 0.0f, 1.0f); auto emissionSampleCdf = getCdfFunction(emissionSamplePdf,
  // 0.0f, 1.0f, true);

  // for(size_t i = 0; i < 10000; ++i){
  //   csvWriter.add("emissionSampleFunction", float(i)/10000,
  //   emissionSampleFunction(float(i)/10000));
  //   csvWriter.add("emissionSampleCdf", float(i)/10000,
  //   emissionSampleCdf(float(i)/10000)); csvWriter.add("emissionSamplePdf",
  //   float(i)/10000, emissionSamplePdf(float(i)/10000));
  //   csvWriter.add("emissionCdf", float(i)/10000,
  //   emissionCdf(float(i)/10000));
  // }

  // csvWriter.write();

  LOG("Start");

  vecn<float, 2> params;

  auto mirrorShapeParabola = [&](float x) { 
    vec2 point = {x, params[0] * x * x + params[1]};
    return point;
  };

  auto mirrorShapeBezier = [&](float x) { 
    vec2 start = {0.25f, 0.0f};
    vec2 paramPoint1 = {0.4f, params[0]};
    vec2 paramPoint2 = {0.6f, params[1]};
    vec2 end = {0.75f, 2.0f};
    vec2 point = bezier(start, paramPoint1, paramPoint2, end, x);
    return point;
  };

  auto mirror = make_shared<Mirror2D>(
      Mirror2D({2.0f, 0.0f}, {-1.0f, 0.0f}, mirrorShapeBezier, 100));

  auto crystal = make_shared<Grid2D>(
      Grid2D({1.0f, 0.0f}, {-0.5f, -0.1f}, {0.5f, 0.1f}, 100, 20,
             [&](Ray2D &ray, float distance, float &cell) {
               // Lambert law of absorption
               float alpha = absorptionSpectrum(ray.wavelength);
               float remainingPower = ray.power * exp(-alpha * distance);
               float absorbedPower = ray.power - remainingPower;
               cell += absorbedPower;
               ray.power = remainingPower;
             }));

  auto lens =
      make_shared<Lens2D>(Lens2D({-2.0f, 0.0f}, {1.0f, 0.0f}, 0.5f, 1.0f));

  Scene2D scene;

  scene.add(mirror);
  scene.add(crystal);
  scene.add(lens);

  RNG::StratifiedSampler1D originSampler;
  RNG::ImportanceSampler1D absorptionImpSampler(absorptionSpectrum, 300.0f,
                                                1000.0f);

  scene.generateDirectionalRays({-5.0f, 0.0f}, 0.5f, {1.0f, 0.0f}, 1000.0f,
                                10000, originSampler, absorptionImpSampler,
                                emissionSpectrum);

  float generatedPower = 0.0f;
  for (const auto &ray : scene.startrays) {
    csvWriter.add("generatedSpectrum", ray.wavelength, ray.power);
    generatedPower += ray.power;
  }

  csvWriter.write();

  LOG("Preprocessing");

  vector<vector<Ray2D>> rays;

  auto trace = [&](const vecn<float, 2> &currentParams) {
    params = currentParams;
    mirror->rebuild();
    rays = scene.trace(4);
    float functional = -crystal->sum();
    crystal->reset();
    return functional;
  };
  
  auto traceVar = [&](const vecn<float, 2> &currentParams) {
    params = currentParams;
    mirror->rebuild();
    rays = scene.trace(4);
    float functional = crystal->var();
    crystal->reset();
    return functional;
  };

  // Optimization Algorithm

  cout << "Mirror Optimizer: " << endl;

  auto solutions = mads<2>(trace, traceVar, {0.0f, 0.0f}, {0.0f, 0.0f}, {2.0f, 2.0f});
  // auto solutions = gradientDescent<2>(trace, {0.0f, 2.0f}, 10, {0.2f, 0.2f});
  if(solutions.empty()){
    DEBUG("Error: No solutions found");
    return -1;
  }

  auto minimizingParameters = solutions[0];

  LOG("Tracing");

  params = minimizingParameters;
  mirror->rebuild();
  rays = scene.trace(4);

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
