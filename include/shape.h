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
  virtual IntersectResult2D intersect(Ray2D &ray) {
    return IntersectResult2D();
  }
  virtual vector<vec4> lineRepresentation() { return {}; }
};

class Shape3D {
public:
  virtual IntersectResult3D intersect(Ray3D &ray) = 0;
};

class Line2D : public Shape2D {
public:
  vec2 a, b;

  Line2D(const vec2 &_a, const vec2 &_b) : a(_a), b(_b) {}

  inline IntersectResult2D intersect(Ray2D &ray) {

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
        ray.t = t;
        ray.hit = true;
      }
    }
    return result;
  }

  vector<vec4> lineRepresentation();
};

ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult);
ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult);
ostream &operator<<(ostream &stream, const Line2D &line);
