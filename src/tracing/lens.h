#pragma once

#include "math/allmath.h"
#include "geometric.hpp"
#include "gtx/transform.hpp"
#include "tracing/object.h"
#include "tracing/shape.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

class Lens2D : public Object2D {
public:
  double _radius;
  double _focalLength;
  Lens2D(const dvec2 &pos, const dvec2 &up, double radius, double focalLength)
      : Object2D(0, pos, up), _radius(radius), _focalLength(focalLength) {
    init();
  };

  vector<shared_ptr<Shape2D>> build();

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays);
};
