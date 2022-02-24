#pragma once

#include "allmath.h"
#include "ray.h"

struct IntersectResult2D {
  float tEnter;
  float tLeave;
  bool hit = false;
  vec2 normalEnter;
  vec2 normalLeave;
};

struct IntersectResult3D {
  float tEnter;
  float tLeave;
  bool hit = false;
  vec3 normalEnter;
  vec3 normalLeave;
};

struct AABB2D {
  vec2 bmin, bmax;
  AABB2D(const vector<vec2> &points = {{0.0f, 0.0f}, {0.0f, 0.0f}});
  vec2 getMidPoint() const;
  bool isInside(const vec2 &point) const;
};

class Shape2D {
public:
  AABB2D aabb;

  virtual IntersectResult2D intersect(const Ray2D &ray) const {
    return IntersectResult2D();
  }
  virtual vector<vec4> lineRepresentation() const { return {}; }
};

class Shape3D {
public:
  virtual IntersectResult3D intersect(Ray3D &ray) = 0;
};

class Line2D : public Shape2D {
public:
  vec2 a, b;

  Line2D(const vec2 &_a, const vec2 &_b);
  IntersectResult2D intersect(const Ray2D &ray) const;
  vector<vec4> lineRepresentation() const;
};

class BoundingBox2D : public Shape2D {
public:
  BoundingBox2D(const vec2 &_bmin = {0.0f, 0.0f},
                const vec2 &_bmax = {0.0f, 0.0f});
  BoundingBox2D(const vector<vec2> &points);
  BoundingBox2D(const AABB2D &_aabb);
  BoundingBox2D(const vector<AABB2D> &aabbs);
  BoundingBox2D(const vector<BoundingBox2D> &aabbs);

  IntersectResult2D intersect(const Ray2D &ray) const;
  IntersectResult2D intersectCheck(const Ray2D &ray) const;

  vector<vec4> lineRepresentation() const;
};

class Sphere2D : public Shape2D {
public:
  vec2 center;
  float radius;
  vec2 facing;
  float maxAngle;

  Sphere2D(const vec2 &_center, float _radius,
           const vec2 &_facing = {0.0f, 1.0f}, float _maxAngle = 2.0f * M_PI);
  IntersectResult2D intersect(const Ray2D &ray) const;
  vector<vec4> lineRepresentation() const;
};

ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult);
ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult);
ostream &operator<<(ostream &stream, const Line2D &line);
