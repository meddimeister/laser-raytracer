#pragma once

#include "object.h"
#include "shape.h"

vector<shared_ptr<Shape2D>> build(const dvec2 &_pos, const dvec2 &_bmin,
                                  const dvec2 &_bmax);

class Grid2D : public Object2D {
  friend class VTKWriter;

private:
  dvec2 cornerMin, cornerMax;
  int maxX, maxY;
  double dx, dy;
  vector<double> data;
  function<void(Ray2D &, double, double &)> cellAction;
  function<void(Ray2D &, const IntersectResult2D&)> hitAction;
  function<double(double)> refractiveIndexFunction;

public:
  Grid2D(const dvec2 &_pos, const dvec2 &_bmin, const dvec2 &_bmax, int _maxX,
         int _maxY, function<void(Ray2D &, double, double &)> _cellAction,
         function<void(Ray2D &, const IntersectResult2D&)> _hitAction,
         function<double(double)> _refractiveIndexFunction)
      : Object2D(build(_pos, _bmin, _bmax), 0, _pos), maxX(_maxX), maxY(_maxY),
        data(maxX * maxY, 0.0), cellAction(_cellAction), hitAction(_hitAction),
        refractiveIndexFunction(_refractiveIndexFunction) {
    cornerMin = root->box->aabb.bmin;
    cornerMax = root->box->aabb.bmax;
    dx = (cornerMax.x - cornerMin.x) / maxX;
    dy = (cornerMax.y - cornerMin.y) / maxY;
  }

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
