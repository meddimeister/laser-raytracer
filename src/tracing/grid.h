#pragma once

#include "tracing/object.h"
#include "tracing/shape.h"
#include <memory>

class Grid2D : public Object2D {
  friend class VTKWriter;

private:
  dvec2 _bmin, _bmax;
  dvec2 _cornerMin, _cornerMax;
  int _maxX, _maxY;
  double _dx, _dy;
  vector<double> _data;
  vecn<double, 4> _sellmeierCoeff;
  function<void(Ray2D &, double, double &)> _cellAction;
  function<void(Ray2D &, const IntersectResult2D &)> _hitAction;

public:
  Grid2D(const dvec2 &pos, const dvec2 &bmin, const dvec2 &bmax, int maxX,
         int maxY, const vecn<double, 4> sellmeierCoeff,
         function<void(Ray2D &, double, double &)> cellAction,
         function<void(Ray2D &, const IntersectResult2D &)> hitAction)
      : Object2D(pos, {0.0, 1.0}, 0), _bmin(bmin), _bmax(bmax), _maxX(maxX), _maxY(maxY),
        _data(maxX * maxY, 0.0), _sellmeierCoeff(sellmeierCoeff),
        _cellAction(cellAction), _hitAction(hitAction) {
    init();
    _cornerMin = root->box->aabb.bmin;
    _cornerMax = root->box->aabb.bmax;
    _dx = (_cornerMax.x - _cornerMin.x) / _maxX;
    _dy = (_cornerMax.y - _cornerMin.y) / _maxY;
  }

  vector<shared_ptr<Shape2D>> build();

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays);

  double sum();
  double avg();
  double var();
  double stddev();

  void reset();

  friend ostream &operator<<(ostream &stream, const Grid2D &grid);
};

ostream &operator<<(ostream &stream, const Grid2D &grid);
