#include "grid.h"
#include <iostream>

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin,
                                  const vec2 &_bmax) {
  vector<shared_ptr<Shape2D>> boxes;
  boxes.push_back(make_shared<BoundingBox2D>(_pos + _bmin, _pos + _bmax));
  return boxes;
}

void Grid2D::action(Ray2D &ray, const IntersectResult2D &result,
                    vector<Ray2D> &createdRays) {
  
  hitAction(ray, result);
  ray.terminate(result.tEnter);
  auto [ray_reflect_in, ray_transmit_in] =
      ray.refract(result.tEnter, result.normalEnter, 1.0f, refractiveIndexFunction(ray.wavelength));

  vec2 enter = ray_transmit_in.origin + 0.0f * ray_transmit_in.direction;

  float xf = (enter.x - cornerMin.x) / dx;
  float yf = (enter.y - cornerMin.y) / dy;

  int x = xf;
  int y = yf;

  if (x == maxX && ray_transmit_in.direction.x < 0.0f) {
    x = x - 1;
  }

  if (y == maxY && ray_transmit_in.direction.y < 0.0f) {
    y = y - 1;
  }

  int stepX = ray_transmit_in.direction.x >= 0.0f ? 1 : -1;
  int stepY = ray_transmit_in.direction.y >= 0.0f ? 1 : -1;

  int nextX = stepX > 0 ? x + stepX : x;
  int nextY = stepY > 0 ? y + stepY : y;

  float tMaxX = abs(cornerMin.x + nextX * dx - ray_transmit_in.origin.x) /
                abs(ray_transmit_in.direction.x);
  float tMaxY = abs(cornerMin.y + nextY * dy - ray_transmit_in.origin.y) /
                abs(ray_transmit_in.direction.y);

  float tDeltaX = dx / abs(ray_transmit_in.direction.x);
  float tDeltaY = dy / abs(ray_transmit_in.direction.y);

  float tLast = 0.0f;
  float tSum = 0.0f;

  while (x >= 0 && x < maxX && y >= 0 && y < maxY) {
    float tTravel;
    float xCurrent = x;
    float yCurrent = y;
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
    float distance = length(tTravel * ray_transmit_in.direction);
    cellAction(ray_transmit_in, distance, data[yCurrent * maxX + xCurrent]);
  }

  vec2 normalLeave;
  if (x < 0)
    normalLeave.x = -1;
  else if (x >= maxX)
    normalLeave.x = 1;
  else if (y < 0)
    normalLeave.y = -1;
  else if (y >= maxY)
    normalLeave.y = 1;

  ray_transmit_in.terminate(tSum);

  auto [ray_reflect_out, ray_transmit_out] =
      ray_transmit_in.refract(tSum, normalLeave, 5.0f, 1.0f);

  // createdRays.push_back(ray_reflect_in);
  createdRays.push_back(ray_transmit_in);
  // createdRays.push_back(ray_reflect_out);
  // createdRays.push_back(ray_transmit_out);
}

float Grid2D::sum() {
  float sum = 0.0f;
  for (const auto &cell : data) {
    sum += cell;
  }
  return sum;
}

float Grid2D::avg() { return sum() / data.size(); }

float Grid2D::var() {
  float var = 0.0f;
  float avg = this->avg();
  for (const auto &cell : data) {
    var += (cell - avg) * (cell - avg);
  }
  return var;
}

float Grid2D::stddev() { return sqrt(var()); }

void Grid2D::reset() {
  for (auto &cell : data) {
    cell = 0.f;
  }
}

ostream &operator<<(ostream &stream, const Grid2D &grid) {
  cout << "Grid2D: {grid: [" << endl;
  for (int y = 0; y < grid.maxY; ++y) {
    for (int x = 0; x < grid.maxX; ++x) {
      cout << grid.data[y * grid.maxX + x] << "\t";
    }
    cout << endl;
  }
  return cout << "]}";
}
