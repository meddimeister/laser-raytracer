#include "ray.h"
#include "gtx/rotate_vector.hpp"
#include <vector>

ostream &operator<<(ostream &stream, const Ray &ray) {
  return stream << "Ray: {"
                << "origin: [" << ray.origin.x << "," << ray.origin.y << ","
                << ray.origin.z << "]"
                << ", "
                << "direction: [" << ray.direction.x << "," << ray.direction.y
                << "," << ray.direction.z << "]"
                << "}";
}

vector<Ray> generatePointRays(
    vec3 origin, vec3 direction, float angle, unsigned int count,
    const function<vec2(unsigned int, unsigned int)> &&angleSampler) {

  vector<Ray> rays;

  vec3 up = {0.0f, 1.0f, 0.0f};
  vec3 axis1 = cross(direction, up);
  vec3 axis2 = cross(axis1, direction);

  for (unsigned int i = 0; i < count; ++i) {
    vec2 angles = angle * angleSampler(i, count);
    vec3 dir = rotate(direction, angles.x, axis1);
    dir = normalize(rotate(dir, angles.y, axis2));

    rays.push_back(Ray(origin, dir));
  }

  return rays;
}
