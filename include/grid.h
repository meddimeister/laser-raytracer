#pragma once

#include "object.h"

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin,
                                  const vec2 &_bmax);

class Grid2D : public Object2D {
  friend class VTKWriter;

private:
  vec2 cornerMin, cornerMax;
  int maxX, maxY;
  float dx, dy;
  vector<float> data;
  function<void(Ray2D &, float, float &)> cellAction;
  function<float(float)> refractiveIndexFunction;

public:
  Grid2D(const vec2 &_pos, const vec2 &_bmin, const vec2 &_bmax, int _maxX,
         int _maxY, function<void(Ray2D &, float, float &)> _cellAction, function<float(float)> _refractiveIndexFunction)
      : Object2D(build(_pos, _bmin, _bmax), 0, _pos), maxX(_maxX), maxY(_maxY),
        data(maxX * maxY, 0.0f), cellAction(_cellAction), refractiveIndexFunction(_refractiveIndexFunction) {
    cornerMin = root->box->aabb.bmin;
    cornerMax = root->box->aabb.bmax;
    dx = (cornerMax.x - cornerMin.x) / maxX;
    dy = (cornerMax.y - cornerMin.y) / maxY;
  }

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays);

  float sum();
  float avg();
  float var();
  float stddev();

  void reset();

  friend ostream &operator<<(ostream &stream, const Grid2D &grid);
};

ostream &operator<<(ostream &stream, const Grid2D &grid);
