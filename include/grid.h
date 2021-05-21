#pragma once

#include "object.h"

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin,
                                  const vec2 &_bmax);

class Grid2D : public Object2D {
public:
  vec2 cornerMin, cornerMax;
  int maxX, maxY;
  float dx, dy;
  vector<float> data;

  Grid2D(const vec2 &_pos, const vec2 &_bmin, const vec2 &_bmax, int _maxX,
         int _maxY)
      : Object2D(build(_pos, _bmin, _bmax), 0, _pos), maxX(_maxX), maxY(_maxY),
        data(maxX * maxY, 0.0f) {
    cornerMin = root->box->aabb.bmin;
    cornerMax = root->box->aabb.bmax;
    dx = (cornerMax.x - cornerMin.x) / maxX;
    dy = (cornerMax.y - cornerMin.y) / maxY;
  }

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays) {
    vec2 enter = ray.origin + result.tEnter * ray.direction;

    float xf = (enter.x - cornerMin.x) / dx;
    float yf = (enter.y - cornerMin.y) / dy;

    int x = xf;
    int y = yf;

    if (x == maxX && ray.direction.x < 0.0f){
      x = x - 1;
		}

    if (y == maxY && ray.direction.y < 0.0f){
      y = y - 1;
		}

    int stepX = ray.direction.x >= 0.0f ? 1 : -1;
    int stepY = ray.direction.y >= 0.0f ? 1 : -1;

    int nextX = stepX > 0 ? x + stepX : x;
    int nextY = stepY > 0 ? y + stepY : y;

    float tMaxX = (cornerMin.x + nextX * dx - ray.origin.x) / ray.direction.x;
    float tMaxY = (cornerMin.y + nextY * dy - ray.origin.y) / ray.direction.y;

    float tDeltaX = dx / abs(ray.direction.x);
    float tDeltaY = dy / abs(ray.direction.y);

    float tLast = result.tEnter;

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
      float distance = length(tTravel * ray.direction);
      data[yCurrent * maxX + xCurrent] += distance;
    }

    ACTION_PRESETS::pass(ray, result, createdRays);
  }

  friend ostream &operator<<(ostream &stream, const Grid2D &grid);
};

ostream &operator<<(ostream &stream, const Grid2D &grid);
