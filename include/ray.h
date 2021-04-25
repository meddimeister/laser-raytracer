#include "glm.hpp"
#include <functional>
#include <ostream>
#include <vector>

using namespace std;
using namespace glm;

struct Ray {
  vec3 origin;
  vec3 direction;

  Ray(const vec3 &_origin, const vec3 &_direction)
      : origin(_origin), direction(_direction) {}
};

ostream &operator<<(ostream &stream, const Ray &ray);

vector<Ray> generatePointRays(
    vec3 origin, vec3 direction, float angle, unsigned int count,
    const function<vec2(unsigned int, unsigned int)> &&angleSampler);
