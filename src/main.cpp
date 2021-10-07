#include "grid.h"
#include "lens.h"
#include "log.h"
#include "mirror.h"
#include "random.h"
#include "ray.h"
#include "scene.h"
#include "shape.h"
#include "vtk.h"
#include "csv.h"
#include "optimization.h"
#include "vecn.h"

using namespace std;

int main(int argc, char *argv[])
{
  LOG("Start");

  float a = 0.f;
  float b = 0.f;

  auto mirror = make_shared<Mirror2D>(Mirror2D(
      {2.0f, 0.0f}, {-1.0f, 0.0f}, [&](float x)
      { return a * x * x + b; },
      100));

  auto crystal = make_shared<Grid2D>(
      Grid2D({1.0f, 0.0f}, {-0.5f, -0.1f}, {0.5f, 0.1f}, 100, 20,
             [](Ray2D &ray, float distance, float &cell)
             {
               if (isnan(ray.power))
               {
                 cout << ray;
               }
               float alpha = 2.0f;
               float remainingPower = ray.power * exp(-alpha * distance);
               float absorbedPower = ray.power - remainingPower;
               cell += absorbedPower;
               ray.power = remainingPower;
             }));

  // auto lens = make_shared<Lens2D>(Lens2D({0.25f, 0.0f}, {0.0f, 0.0f}, 0.1f,
  // {0.0f, 0.0f}));

  Scene2D scene;

  scene.add(mirror);
  scene.add(crystal);
  // scene.add(lens);

  RNG::StratifiedSampler1D sampler;
  scene.generatePointRays({0.0f, 0.0f}, {1.0f, 0.0f}, 0.5f, 1000.0f, 10000,
                          sampler);

  LOG("Preprocessing");

  vector<vector<Ray2D>> rays;

  CSVWriter csvWriter("csvOut");
  csvWriter.add("#a b Abs.Power[W]", "absorbed_power");

  auto trace = [&](const vecn<float, 2> &params)
  {
    a = params[0];
    b = params[1];
    mirror->rebuild();
    rays = scene.trace(3);
    float functional = -crystal->sum();
    crystal->reset();
    return functional;
  };

  //Optimization Algorithm

  cout << "Mirror Optimizer: " << endl;

  cout << "Initial gridSearch: " << endl;
  vecn<float, 2> xMin = gridSearch<2>(trace, {0.0f, 0.0f}, 10, {0.2f, 0.2f});
  cout << "Minimum of initial gridSearch: " << trace(xMin) << endl;

  LOG("Initial Search");

  cout << "gradientDescent: " << endl;
  xMin = gradientDescent<2>(trace, xMin);
  cout << "Minimum of gradientDescent: " << trace(xMin) << endl;
  cout << "Minimizing parameters: ";

  for (const auto &param : xMin)
  {
    cout << param << " ";
  }
  cout << endl;

  LOG("Tracing");

  csvWriter.write();

  VTKWriter vtkWriter("vtkOut");
  vtkWriter.add(mirror, "mirror");
  vtkWriter.add(mirror->getAABBs(), "mirror.AABB");
  vtkWriter.add(crystal, "crystal");
  vtkWriter.add(crystal->getAABBs(), "crystal.AABB");
  // vtkWriter.add(lens, "lens");
  // vtkWriter.add(lens->getAABBs(), "lens.AABB");
  vtkWriter.addAsSequence(rays, "rays", 100);
  vtkWriter.addAsComposition(rays, "rays_composition", 100);
  vtkWriter.write();

  LOG("Output");
}
