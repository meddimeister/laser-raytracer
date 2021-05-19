#pragma once

#include "math.h"
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
      : Object2D(build(_pos, _opticalAxis, _shapeFunction, _segments),
                 ACTION_PRESETS::reflect, 4, _pos, _opticalAxis){};
};
