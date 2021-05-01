#include "ray.h"
#include "random.h"
#include "shape.h"
#include "vtk.h"
#include "mirror.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

	vector<Ray2D> rays = generatePointRays({0.0f,0.0f}, {1.0f, 0.0f}, 0.5f, 100, RNG::uniformLine);
  
	Mirror2D mirror({2.0f, 0.0f}, {-1.0f, 0.0f}, [](float x){return 0.5f*x*x;}, 100);

	vector<Ray2D> reflections = mirror.reflect(rays);

	VTKWriter vtkWriter("vtkOut");
	vtkWriter.add(mirror, "mirror");
	vtkWriter.add(rays, "rays0");
	vtkWriter.add(reflections, "rays1");
	vtkWriter.write();
}
