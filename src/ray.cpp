#include "ray.h"
#include "gtx/rotate_vector.hpp"
#include <vector>

ostream &operator<<(ostream &stream, const Ray2D &ray) {
  return stream << "Ray2D: {"
                << "origin: [" << ray.origin.x << "," << ray.origin.y << "]"
                << ", "
                << "direction: [" << ray.direction.x << "," << ray.direction.y
                << "]"
                << ", "
                << "power: " << ray.power
                << ", "
                << "wavelength: " << ray.wavelength
                << "}";
}
