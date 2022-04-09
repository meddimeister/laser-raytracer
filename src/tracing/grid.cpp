#include "tracing/grid.h"
#include <iostream>

vector<shared_ptr<Shape2D>> Grid2D::build() {
  vector<shared_ptr<Shape2D>> boxes;
  boxes.push_back(make_shared<BoundingBox2D>(pos + _bmin, pos + _bmax));
  return boxes;
}

void Grid2D::actionEnter(Ray2D &ray, const IntersectResult2D &result) {
  _hitAction(ray, result);
}

void Grid2D::actionTransmit(Ray2D &ray, const IntersectResult2D &result) {
  dvec2 enter = ray.origin + 0.0 * ray.direction;

  double xf = (enter.x - _cornerMin.x) / _dx;
  double yf = (enter.y - _cornerMin.y) / _dy;

  int x = xf;
  int y = yf;

  if (x == _maxX && ray.direction.x < 0.0) {
    x = x - 1;
  }

  if (y == _maxY && ray.direction.y < 0.0) {
    y = y - 1;
  }

  int stepX = ray.direction.x >= 0.0 ? 1 : -1;
  int stepY = ray.direction.y >= 0.0 ? 1 : -1;

  int nextX = stepX > 0 ? x + stepX : x;
  int nextY = stepY > 0 ? y + stepY : y;

  double tMaxX =
      abs(_cornerMin.x + nextX * _dx - ray.origin.x) / abs(ray.direction.x);
  double tMaxY =
      abs(_cornerMin.y + nextY * _dy - ray.origin.y) / abs(ray.direction.y);

  double tDeltaX = _dx / abs(ray.direction.x);
  double tDeltaY = _dy / abs(ray.direction.y);

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
    double distance = length(tTravel * ray.direction);
    _cellAction(ray, distance, _data[yCurrent * _maxX + xCurrent]);
  }
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