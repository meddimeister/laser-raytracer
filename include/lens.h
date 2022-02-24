#pragma once

#include "allmath.h"
#include "geometric.hpp"
#include "gtx/transform.hpp"
#include "object.h"
#include "shape.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_opticalAxis,
                                  float _radius);

class Lens2D : public Object2D {
public:
  float radius;
  float focalLength;
  Lens2D(const vec2 &_pos, const vec2 &_opticalAxis, float _radius,
         float _focalLength)
      : Object2D(build(_pos, _opticalAxis, _radius), 0, _pos, _opticalAxis),
        radius(_radius), focalLength(_focalLength){};

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays) {
    // thin lens paper chapter single lens
    float alpha = orientedAngle(ray.direction, up);
    vec2 focalPoint = pos - focalLength * normalize(up);
    vec2 hitPoint = ray.origin + result.tEnter * ray.direction;
    float gamma = orientedAngle(up, normalize(hitPoint - focalPoint));
    float beta = atan(tan(alpha) - tan(gamma));
    vec2 ori = hitPoint;
    vec2 dir = rotate(up, beta);
    ori += numeric_limits<float>::epsilon() * dir;
    ray.terminate(result.tEnter);
    createdRays.push_back(Ray2D(ori, dir, ray.power));
  }
};
