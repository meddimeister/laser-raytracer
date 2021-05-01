#pragma once

#include "math.h"
#include "shape.h"
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

class Object2D {
protected:
  vector<shared_ptr<Shape2D>> shapes;
  vec2 pos;
  vec2 up;
  vec2 scale;

public:
  Object2D(const vec2 &_pos = {0.0f, 0.0f}, const vec2 &_up = {0.0f, 1.0f},
           const vec2 &_scale = {1.0f, 1.0f})
      : pos(_pos), up(_up), scale(_scale) {}

  void setPos(const vec2 &_pos);
  void setUp(const vec2 &_up);
  void setScale(const vec2 &_scale);

  const vector<shared_ptr<Shape2D>> &getShapes() const;
  vec2 getPos() const;
  vec2 getUp() const;
  vec2 getScale() const;

  IntersectResult2D intersect(Ray2D &ray);
  vector<IntersectResult2D> intersect(vector<Ray2D> &ray);

  vector<Ray2D> reflect(vector<Ray2D> &ray);
};
