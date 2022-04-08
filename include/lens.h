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

vector<shared_ptr<Shape2D>> build(const dvec2 &_pos, const dvec2 &_opticalAxis,
                                  double _radius);

class Lens2D : public Object2D {
public:
  double _radius;
  double _focalLength;
  Lens2D(const dvec2 &pos, const dvec2 &up, double radius, double focalLength)
      : Object2D({}, 0, pos, up), _radius(radius), _focalLength(focalLength) {
    init();
  };

  vector<shared_ptr<Shape2D>> build() {
    vector<shared_ptr<Shape2D>> shapes;
    dvec2 a = pos + _radius * normalize(rotate(up, 0.5 * double(M_PI)));
    dvec2 b = pos + _radius * normalize(rotate(up, -0.5 * double(M_PI)));
    shapes.push_back(make_shared<Line2D>(Line2D(a, b)));
    return shapes;
  }

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays) {
    // thin lens paper chapter single lens
    double alpha = orientedAngle(ray.direction, up);
    dvec2 focalPoint = pos - _focalLength * normalize(up);
    dvec2 hitPoint = ray.origin + result.tEnter * ray.direction;
    double gamma = orientedAngle(up, normalize(hitPoint - focalPoint));
    double beta = atan(tan(alpha) - tan(gamma));
    dvec2 ori = hitPoint;
    dvec2 dir = normalize(rotate(up, beta));
    ori += 20.0 * numeric_limits<double>::epsilon() * dir;
    ray.terminate(result.tEnter);
    createdRays.push_back(Ray2D(ori, dir, ray.power, ray.wavelength));
  }
};
