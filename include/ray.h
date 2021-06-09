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
  bool hit = false;
  float t;

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
	float power;
  bool terminated = false;
  float terminatedAt;

  Ray2D(const vec2 &_origin, const vec2 &_direction, float _power)
      : origin(_origin), direction(_direction), power(_power) {}

  Ray2D reflect(float t, const vec2 &normal) const {
    vec2 ori = origin + t * direction;
    vec2 dir = glm::reflect(direction, normal);
    return Ray2D(ori, dir, power);
  }

	void terminate(float t){
		terminated = true;
		terminatedAt = t;
	}
};

ostream &operator<<(ostream &stream, const Ray3D &ray);
ostream &operator<<(ostream &stream, const Ray2D &ray);
