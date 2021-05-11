#pragma once

#include "math.h"
#include "object.h"
#include "shape.h"
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_opticalAxis,
           const function<float(float)> _shapeFunction, int _segments);

class Mirror2D : public Object2D {
public:
  Mirror2D(const vec2 &_pos, const vec2 &_opticalAxis,
           const function<float(float)> _shapeFunction, int _segments)
      : Object2D(
            REFLECT,
            build(_pos, _opticalAxis, _shapeFunction, _segments),
            _pos, _opticalAxis){};
};
