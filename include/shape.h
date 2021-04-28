#pragma once

#include "math.h"
#include "ray.h"

struct IntersectResult2D {
	float t;
	bool hit = false;
	vec2 normal;
};

struct IntersectResult3D {
	float t;
	bool hit = false;
	vec3 normal;
};

class Shape2D {
public:
  virtual IntersectResult2D intersect(const Ray2D &ray) = 0;
};

class Shape3D {
public:
  virtual IntersectResult3D intersect(const Ray3D &ray) = 0;
};

class Line2D : public Shape2D {
public:
  vec2 a, b;

  Line2D(const vec2 &_a, const vec2 &_b) : a(_a), b(_b) {}

  inline IntersectResult2D intersect(const Ray2D &ray) {

    IntersectResult2D result;

    vec2 bma = b - a;
    vec2 ame = a - ray.origin;
    float dxbma = cross(ray.direction, bma);
    float alpha = cross(ame, ray.direction) / dxbma;

    if (alpha >= 0.0f && alpha <= 1.0f) {
      float t = cross(ame, bma) / dxbma;
      if (t >= 0.0f) {
        result.t = t;
        result.hit = true;
        result.normal = normalize(vec2(a.y - b.y, b.x - a.x));
      }
    }
    return result;
  }
};

ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult);
ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult);
ostream &operator<<(ostream &stream, const Line2D &line);
