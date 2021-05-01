#pragma once

#include "object.h"
#include "ray.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <shape.h>
#include <string>
#include <unordered_map>

using namespace std;

class VTKWriter {
private:
  unordered_map<string, vector<shared_ptr<Shape2D>>> shapes2D;
  unordered_map<string, vector<Ray2D>> rays2D;
  string outputDirectory;

public:
  VTKWriter(const string &_outputDirectory)
      : outputDirectory(_outputDirectory) {}

  void add(const shared_ptr<Shape2D> &shape, const string &name);
  void add(const Ray2D &ray, const string &name);
  void add(const vector<shared_ptr<Shape2D>> &shapes, const string &name);
  void add(const vector<Ray2D> &rays, const string &name);
  void add(const Object2D &object, const string &name);
  void add(const vector<Object2D> &objects, const string &name);

  void write();
};
