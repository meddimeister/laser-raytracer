#pragma once

#include "math/allmath.h"
#include "tracing/object.h"
#include "tracing/shape.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

class Mirror2D : public Object2D {
private:
  function<dvec2(double)> _shapeFunction;
  int _segments;

public:
  Mirror2D(const dvec2 &pos, const dvec2 &up,
           function<dvec2(double)> shapeFunction, int segments)
      : Object2D(4, pos, up), _shapeFunction(shapeFunction),
        _segments(segments) {
    init();
  };

  vector<shared_ptr<Shape2D>> build();

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays);
};
