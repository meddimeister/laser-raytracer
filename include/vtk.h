#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <shape.h>
#include <ray.h>

using namespace std;

class VTKWriter {
private:
	vector<Line2D> lines2D;
	vector<Ray2D> rays2D;

public:
	void add(const Line2D &line);
	void add(const Ray2D &ray);

  void write(const string &outputDirectory);
};
