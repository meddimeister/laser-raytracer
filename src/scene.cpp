#include "scene.h"
#include <cmath>

void Scene2D::add(const shared_ptr<Object2D> &object) {
  objects.push_back(object);
}

void
Scene2D::generatePointRays(const vec2 &origin, const vec2 &direction, float maxAngle,
                           unsigned int count,
                           const function<float()> &&lineDistribution) {

  for (unsigned int i = 0; i < count; ++i) {
    float angle = maxAngle * lineDistribution();
    vec2 dir = rotate(direction, angle);
    dir = normalize(dir);

    startrays.push_back(Ray2D(origin, dir));
  }
}

vector<vector<Ray2D>> Scene2D::trace(unsigned int depth) {

  vector<vector<Ray2D>> allrays(depth);
  allrays[0] = startrays;

  for (unsigned int d = 1; d < depth; ++d) {
    vector<Ray2D> reflections;

    for (auto &ray : allrays[d - 1]) {

      IntersectResult2D resultFinal;
      resultFinal.tEnter = MAXFLOAT;
      shared_ptr<Object2D> objectFinal;

      for (auto &object : objects) {
        IntersectResult2D result = object->intersect(ray);
        if (result.hit) {
          if (object->getType() != PASS && result.tEnter < resultFinal.tEnter) {
            objectFinal = object;
            resultFinal = result;
          }
        }
      }

      switch (objectFinal->getType()) {
      case PASS:
        break;
      case ABSORB:
        ray.hit = true;
        ray.t = resultFinal.tEnter;
        break;
      case REFLECT:
        ray.hit = true;
        ray.t = resultFinal.tEnter;
        reflections.push_back(
            ray.reflect(resultFinal.tEnter, resultFinal.normalEnter));
        break;
      }
    }
    allrays[d] = move(reflections);
  }
  return allrays;
}
