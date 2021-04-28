#include "ray.h"
#include "random.h"
#include "shape.h"
#include "vtk.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

	vector<Ray2D> rays = generatePointRays({0.0f,0.0f}, {1.0f, 0.0f}, 0.5f, 10, RNG::uniformLine);
  
	Line2D line({0.0f, -1.0f}, {2.0f, 1.0f});

	vector<Ray2D> reflections = line.reflect(rays);

	VTKWriter vtkWriter;
	vtkWriter.add(line);
	vtkWriter.add(rays);
	vtkWriter.add(reflections);
	vtkWriter.write("vtkOut");
}
