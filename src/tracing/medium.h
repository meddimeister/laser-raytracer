#pragma once

#include "tracing/object.h"
#include "tracing/shape.h"

using namespace std;

class Medium2D : public Object2D {

private:
  vecn<double, 4> _sellmeierCoeff;

public:
  Medium2D(
      const dvec2 &pos, const dvec2 &up, const vecn<double, 4> sellmeierCoeff,
      unsigned int subdivisions = 0)
      : Object2D(pos, up, subdivisions), _sellmeierCoeff(sellmeierCoeff) {
    init();
  }

  virtual vector<shared_ptr<Shape2D>> build() = 0;

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays);

  virtual void actionEnter(Ray2D &ray, const IntersectResult2D &result) = 0;
  virtual void actionTransmit(Ray2D &ray, const IntersectResult2D &result) = 0;
};