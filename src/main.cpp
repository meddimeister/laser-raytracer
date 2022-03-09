#include "csv.h"
#include "debugutils.h"
#include "functionutils.h"
#include "grid.h"
#include "lens.h"
#include "log.h"
#include "mirror.h"
#include "nomadbinding.h"
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

  float a = 0.f;
  float b = 0.f;

  auto mirror = make_shared<Mirror2D>(Mirror2D(
      {2.0f, 0.0f}, {-1.0f, 0.0f}, [&](float x) { return a * x * x + b; },
      100));

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

  auto trace = [&](const vecn<float, 2> &params) {
    a = params[0];
    b = params[1];
    mirror->rebuild();
    rays = scene.trace(4);
    float functional = -crystal->sum();
    crystal->reset();
    return functional;
  };

  // Optimization Algorithm

  cout << "Mirror Optimizer: " << endl;

  auto solutions = mads<2>(trace, {0.0f, 0.0f}, {0.0f, -5.0f}, {10.0f, 0.4f});
  // auto solutions = gradientDescent<2>(trace, {0.0f, 2.0f}, 10, {0.2f, 0.2f});

  auto xMin = solutions[0];

  LOG("Tracing");

  a = xMin[0];
  b = xMin[1];
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
