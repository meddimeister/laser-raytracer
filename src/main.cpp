#include "grid.h"
#include "mirror.h"
#include "random.h"
#include "ray.h"
#include "scene.h"
#include "shape.h"
#include "vtk.h"
#include "log.h"

using namespace std;

int main(int argc, char *argv[]) {

	LOG("Start");

  auto mirror = make_shared<Mirror2D>(Mirror2D(
      {2.0f, 0.0f}, {-1.0f, 0.0f}, [](float x) { return 0.5f * x * x; }, 100));

  auto crystal = make_shared<Grid2D>(
      Grid2D({1.0f, 0.0f}, {-0.5f, -0.1f}, {0.5f, 0.1f}, 100, 20));

  Scene2D scene;

  scene.add(mirror);
  scene.add(crystal);

  RNG::StratifiedSampler1D sampler;
  scene.generatePointRays({0.0f, 0.0f}, {1.0f, 0.0f}, 0.5f, 10000, sampler);

	LOG("Preprocessing");

  vector<vector<Ray2D>> rays = scene.trace(3);

	LOG("Tracing");

  VTKWriter vtkWriter("vtkOut");
  vtkWriter.add(mirror, "mirror");
  vtkWriter.add(mirror->getAABBs(), "mirror.AABB");
  vtkWriter.add(crystal, "crystal");
  vtkWriter.add(crystal->getAABBs(), "crystal.AABB");
  vtkWriter.addAsSequence(rays, "rays", 100);
  vtkWriter.addAsComposition(rays, "rays_composition", 100);
  vtkWriter.write();

	LOG("Output");
}
