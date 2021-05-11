#include "ray.h"
#include "random.h"
#include "shape.h"
#include "vtk.h"
#include "mirror.h"
#include "crystal.h"
#include <iostream>
#include "scene.h"

using namespace std;

int main(int argc, char *argv[]) {

	Mirror2D mirror({2.0f, 0.0f}, {-1.0f, 0.0f}, [](float x){return 0.5f*x*x;}, 100);

	Crystal2D crystal({1.0f, 0.0f}, {-0.5f, -0.1f}, {0.5f, 0.1f});

	Scene2D scene;
	
	scene.add(make_shared<Mirror2D>(mirror));
	scene.add(make_shared<Crystal2D>(crystal));

	scene.generatePointRays({0.0f,0.0f}, {1.0f, 0.0f}, 0.5f, 100, RNG::uniformLine);

	vector<vector<Ray2D>> rays = scene.trace(3);

	VTKWriter vtkWriter("vtkOut");
	vtkWriter.add(mirror, "mirror");
	vtkWriter.add(mirror.getAABBs(), "mirror.AABB");
	vtkWriter.add(crystal, "crystal");
	vtkWriter.add(crystal.getAABBs(), "crystal.AABB");
	vtkWriter.addAsSequence(rays, "rays");
	vtkWriter.addAsComposition(rays, "rays_composition");
	vtkWriter.write();
}
