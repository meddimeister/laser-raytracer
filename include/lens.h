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
                                  float _thickness, const vec2 &_radii);

class Lens2D : public Object2D {
public:
  Lens2D(const vec2 &_pos, const vec2 &_opticalAxis, float _thickness,
         const vec2 &_radii)
      : Object2D(build(_pos, _opticalAxis, _thickness, _radii), 0,
                 _pos, _opticalAxis){};

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays) {
    ACTION_PRESETS::reflect(ray, result, createdRays);
  }
};
