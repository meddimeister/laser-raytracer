#pragma once

#include "tracing/medium.h"
#include "tracing/shape.h"
#include <memory>

class Grid2D : public Medium2D {
  friend class VTKWriter;

protected:
  dvec2 _bmin, _bmax;
  dvec2 _cornerMin, _cornerMax;
  int _maxX, _maxY;
  double _dx, _dy;
  vector<double> _data;
  function<void(Ray2D &, double, double &)> _cellAction;
  function<void(Ray2D &, const IntersectResult2D &)> _hitAction;
  

public:
  Grid2D(const dvec2 &pos, const dvec2 &bmin, const dvec2 &bmax, int maxX,
         int maxY, const vecn<double, 4> sellmeierCoeff,
         function<void(Ray2D &, double, double &)> cellAction = [](Ray2D &, double, double &){},
         function<void(Ray2D &, const IntersectResult2D &)> hitAction = [](Ray2D &, const IntersectResult2D &){})
      : Medium2D(pos, {0.0, 1.0}, 0, sellmeierCoeff), _bmin(bmin), _bmax(bmax), _maxX(maxX), _maxY(maxY),
        _data(maxX * maxY, 0.0),
        _cellAction(cellAction), _hitAction(hitAction) {
    init();
    _cornerMin = root->box->aabb.bmin;
    _cornerMax = root->box->aabb.bmax;
    _dx = (_cornerMax.x - _cornerMin.x) / _maxX;
    _dy = (_cornerMax.y - _cornerMin.y) / _maxY;
  }

  vector<shared_ptr<Shape2D>> build();

  void actionEnter(Ray2D &ray, const IntersectResult2D &result);
  void actionTransmit(Ray2D &ray, const IntersectResult2D &result);

  double sum();
  double avg();
  double var();
  double stddev();

  void reset();
};
