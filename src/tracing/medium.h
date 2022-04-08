#pragma once

#include "tracing/object.h"
#include "tracing/shape.h"

using namespace std;

class Medium2D : public Object2D {

private:
  vecn<double, 4> _sellmeierCoeff;
  function<void(Ray2D &, const IntersectResult2D &)> _enterAction;
  function<void(Ray2D &)> _leaveAction;

public:
  Medium2D(
      const dvec2 &pos, const dvec2 &up, const vecn<double, 4> sellmeierCoeff,
      function<void(Ray2D &, const IntersectResult2D &)> enterAction =
          [](Ray2D &, const IntersectResult2D &) {},
      function<void(Ray2D &)> leaveAction =
          [](Ray2D &) {},
      unsigned int subdivisions = 0)
      : Object2D(0, pos, up), _sellmeierCoeff(sellmeierCoeff),
        _enterAction(enterAction), _leaveAction(leaveAction) {
    init();
  }

  virtual vector<shared_ptr<Shape2D>> build();

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays);
};