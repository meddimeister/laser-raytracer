#include "scene.h"
#include <cmath>

vector<vector<Ray2D>> Scene2D::trace(vector<Ray2D> &rays, unsigned int depth) {

  vector<vector<Ray2D>> allrays(depth);
  allrays[0] = rays;

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
