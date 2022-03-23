#pragma once

#include "allmath.h"
#include "ray.h"

struct IntersectResult2D {
  double tEnter;
  double tLeave;
  bool hit = false;
  dvec2 normalEnter;
  dvec2 normalLeave;
};

struct AABB2D {
  dvec2 bmin, bmax;
  AABB2D(const vector<dvec2> &points = {{0.0, 0.0}, {0.0, 0.0}});
  dvec2 getMidPoint() const;
  bool isInside(const dvec2 &point) const;
};

class Shape2D {
public:
  AABB2D aabb;

  virtual IntersectResult2D intersect(const Ray2D &ray) const {
    return IntersectResult2D();
  }
  virtual vector<vec4> lineRepresentation() const { return {}; }
};

class Line2D : public Shape2D {
public:
  dvec2 a, b;

  Line2D(const dvec2 &_a, const dvec2 &_b);
  IntersectResult2D intersect(const Ray2D &ray) const;
  vector<vec4> lineRepresentation() const;
};

class BoundingBox2D : public Shape2D {
public:
  BoundingBox2D(const dvec2 &_bmin = {0.0, 0.0},
                const dvec2 &_bmax = {0.0, 0.0});
  BoundingBox2D(const vector<dvec2> &points);
  BoundingBox2D(const AABB2D &_aabb);
  BoundingBox2D(const vector<AABB2D> &aabbs);
  BoundingBox2D(const vector<BoundingBox2D> &aabbs);

  IntersectResult2D intersect(const Ray2D &ray) const;
  IntersectResult2D intersectCheck(const Ray2D &ray) const;

  vector<vec4> lineRepresentation() const;
};

class Sphere2D : public Shape2D {
public:
  dvec2 center;
  double radius;
  dvec2 facing;
  double maxAngle;

  Sphere2D(const dvec2 &_center, double _radius,
           const dvec2 &_facing = {0.0, 1.0}, double _maxAngle = 2.0 * M_PI);
  IntersectResult2D intersect(const Ray2D &ray) const;
  vector<vec4> lineRepresentation() const;
};

ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult);
ostream &operator<<(ostream &stream, const Line2D &line);
