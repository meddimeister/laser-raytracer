#pragma once

#include <fstream>
#include <iostream>
#include <ray.h>
#include <shape.h>
#include <string>
#include <unordered_map>

using namespace std;

class VTKWriter {
private:
  unordered_map<string, vector<Line2D>> lines2D;
  unordered_map<string, vector<Ray2D>> rays2D;
	string outputDirectory;

public:

	VTKWriter(const string &_outputDirectory) : outputDirectory(_outputDirectory) {}

  void add(const Line2D &line, const string &name);
  void add(const Ray2D &ray, const string &name);
  void add(const vector<Line2D> &lines, const string &name);
  void add(const vector<Ray2D> &rays, const string &name);

  void write();
};
