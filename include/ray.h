#pragma once

#include "math.h"
#include <functional>
#include <ostream>
#include <vector>

using namespace std;
using namespace glm;

struct Ray3D {
  vec3 origin;
  vec3 direction;

  Ray3D(const vec3 &_origin, const vec3 &_direction)
      : origin(_origin), direction(_direction) {}

  Ray3D reflect(float t, const vec3 &normal) const {
    vec3 ori = origin + t * direction;
    vec3 dir = glm::reflect(direction, normal);
    return Ray3D(ori, dir);
  }
};

struct Ray2D {
  vec2 origin;
  vec2 direction;

  Ray2D(const vec2 &_origin, const vec2 &_direction)
      : origin(_origin), direction(_direction) {}

  Ray2D reflect(float t, const vec2 &normal) const {
    vec2 ori = origin + t * direction;
    vec2 dir = glm::reflect(direction, normal);
    return Ray2D(ori, dir);
  }
};

ostream &operator<<(ostream &stream, const Ray3D &ray);
ostream &operator<<(ostream &stream, const Ray2D &ray);

vector<Ray3D> generatePointRays(vec3 origin, vec3 direction, float maxAngle,
                                unsigned int count,
                                const function<vec2()> &&diskDistribution);

vector<Ray2D> generatePointRays(vec2 origin, vec2 direction, float maxAngle,
                                unsigned int count,
                                const function<float()> &&lineDistribution);
