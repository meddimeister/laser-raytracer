#include "grid.h"
#include <iostream>

vector<shared_ptr<Shape2D>> build(const dvec2 &_pos, const dvec2 &_bmin,
                                  const dvec2 &_bmax) {
  vector<shared_ptr<Shape2D>> boxes;
  boxes.push_back(make_shared<BoundingBox2D>(_pos + _bmin, _pos + _bmax));
  return boxes;
}

void Grid2D::action(Ray2D &ray, const IntersectResult2D &result,
                    vector<Ray2D> &createdRays) {
  
  _hitAction(ray, result);
  ray.terminate(result.tEnter);
  auto [ray_reflect_in, ray_transmit_in] =
      ray.refract(result.tEnter, result.normalEnter, 1.0, _refractiveIndexFunction(ray.wavelength));

  dvec2 enter = ray_transmit_in.origin + 0.0 * ray_transmit_in.direction;

  double xf = (enter.x - _cornerMin.x) / _dx;
  double yf = (enter.y - _cornerMin.y) / _dy;

  int x = xf;
  int y = yf;

  if (x == _maxX && ray_transmit_in.direction.x < 0.0) {
    x = x - 1;
  }

  if (y == _maxY && ray_transmit_in.direction.y < 0.0) {
    y = y - 1;
  }

  int stepX = ray_transmit_in.direction.x >= 0.0 ? 1 : -1;
  int stepY = ray_transmit_in.direction.y >= 0.0 ? 1 : -1;

  int nextX = stepX > 0 ? x + stepX : x;
  int nextY = stepY > 0 ? y + stepY : y;

  double tMaxX = abs(_cornerMin.x + nextX * _dx - ray_transmit_in.origin.x) /
                abs(ray_transmit_in.direction.x);
  double tMaxY = abs(_cornerMin.y + nextY * _dy - ray_transmit_in.origin.y) /
                abs(ray_transmit_in.direction.y);

  double tDeltaX = _dx / abs(ray_transmit_in.direction.x);
  double tDeltaY = _dy / abs(ray_transmit_in.direction.y);

  double tLast = 0.0;
  double tSum = 0.0;

  while (x >= 0 && x < _maxX && y >= 0 && y < _maxY) {
    double tTravel;
    double xCurrent = x;
    double yCurrent = y;
    if (tMaxX < tMaxY) {
      tTravel = tMaxX - tLast;
      tLast = tMaxX;
      tMaxX = tMaxX + tDeltaX;
      x = x + stepX;
    } else {
      tTravel = tMaxY - tLast;
      tLast = tMaxY;
      tMaxY = tMaxY + tDeltaY;
      y = y + stepY;
    }
    tSum += tTravel;
    double distance = length(tTravel * ray_transmit_in.direction);
    _cellAction(ray_transmit_in, distance, _data[yCurrent * _maxX + xCurrent]);
  }

  dvec2 normalLeave = {0.0, 0.0};
  if (x < 0)
    normalLeave.x = -1;
  else if (x >= _maxX)
    normalLeave.x = 1;
  else if (y < 0)
    normalLeave.y = -1;
  else if (y >= _maxY)
    normalLeave.y = 1;

  ray_transmit_in.terminate(tSum);

  //auto [ray_reflect_out, ray_transmit_out] =
  //    ray_transmit_in.refract(tSum, normalLeave, 5.0, 1.0);

  // createdRays.push_back(ray_reflect_in);
  createdRays.push_back(ray_transmit_in);
  // createdRays.push_back(ray_reflect_out);
  // createdRays.push_back(ray_transmit_out);
}

double Grid2D::sum() {
  double sum = 0.0;
  for (const auto &cell : _data) {
    sum += cell;
  }
  return sum;
}

double Grid2D::avg() { return sum() / _data.size(); }

double Grid2D::var() {
  double var = 0.0;
  double avg = this->avg();
  for (const auto &cell : _data) {
    var += (cell - avg) * (cell - avg);
  }
  return var;
}

double Grid2D::stddev() { return sqrt(var()); }

void Grid2D::reset() {
  for (auto &cell : _data) {
    cell = 0.;
  }
}

ostream &operator<<(ostream &stream, const Grid2D &grid) {
  cout << "Grid2D: {grid: [" << endl;
  for (int y = 0; y < grid._maxY; ++y) {
    for (int x = 0; x < grid._maxX; ++x) {
      cout << grid._data[y * grid._maxX + x] << "\t";
    }
    cout << endl;
  }
  return cout << "]}";
}
