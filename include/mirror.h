#pragma once

#include "allmath.h"
#include "object.h"
#include "shape.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_opticalAxis,
                                  const function<float(float)> _shapeFunction,
                                  int _segments);

class Mirror2D : public Object2D {
public:
  Mirror2D(const vec2 &_pos, const vec2 &_opticalAxis,
           function<float(float)> _shapeFunction, int _segments)
      : Object2D(build(_pos, _opticalAxis, _shapeFunction, _segments), 4, _pos, _opticalAxis){};

  void action(Ray2D &ray, const IntersectResult2D &result,
                      vector<Ray2D> &createdRays) {
		ACTION_PRESETS::reflect(ray, result, createdRays);
  }
};
