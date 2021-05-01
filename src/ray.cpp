#include "ray.h"
#include "gtx/rotate_vector.hpp"
#include <vector>

ostream &operator<<(ostream &stream, const Ray3D &ray) {
  return stream << "Ray3D: {"
                << "origin: [" << ray.origin.x << "," << ray.origin.y << ","
                << ray.origin.z << "]"
                << ", "
                << "direction: [" << ray.direction.x << "," << ray.direction.y
                << "," << ray.direction.z << "]"
                << "}";
}

ostream &operator<<(ostream &stream, const Ray2D &ray) {
  return stream << "Ray2D: {"
                << "origin: [" << ray.origin.x << "," << ray.origin.y << "]"
                << ", "
                << "direction: [" << ray.direction.x << "," << ray.direction.y
                << "]"
                << "}";
}

vector<Ray3D> generatePointRays(vec3 origin, vec3 direction, float maxAngle,
                                unsigned int count,
                                const function<vec2()> &&diskDistribution) {

  vector<Ray3D> rays;

  vec3 up = {0.0f, 1.0f, 0.0f};
  vec3 axis1 = cross(direction, up);
  vec3 axis2 = cross(axis1, direction);

  for (unsigned int i = 0; i < count; ++i) {
    vec2 angles = maxAngle * diskDistribution();
    vec3 dir = rotate(direction, angles.x, axis1);
    dir = normalize(rotate(dir, angles.y, axis2));

    rays.push_back(Ray3D(origin, dir));
  }

  return rays;
}

vector<Ray2D> generatePointRays(vec2 origin, vec2 direction, float maxAngle,
                                unsigned int count,
                                const function<float()> &&lineDistribution) {

  vector<Ray2D> rays;

  for (unsigned int i = 0; i < count; ++i) {
    float angle = maxAngle * lineDistribution();
    vec2 dir = rotate(direction, angle);
    dir = normalize(dir);

    rays.push_back(Ray2D(origin, dir));
  }

  return rays;
}
