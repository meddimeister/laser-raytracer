#include "scene.h"
#include <cmath>
#include <map>
#include <iostream>

void Scene2D::add(const shared_ptr<Object2D> &object) {
  objects.push_back(object);
}

void Scene2D::generatePointRays(const vec2 &origin, const vec2 &direction,
                                float maxAngle, float totalPower, unsigned int count,
                                RNG::Sampler<float> &sampler) {

  sampler.init(count);
  for (unsigned int i = 0; i < count; ++i) {
    float angle = maxAngle * (2.0f * sampler.next() - 1.0f);
    vec2 dir = rotate(direction, angle);
    dir = normalize(dir);

		float power = totalPower/count;

    startrays.push_back(Ray2D(origin, dir, power));
  }
}

vector<vector<Ray2D>> Scene2D::trace(unsigned int depth) {

  vector<vector<Ray2D>> allrays(depth);
  allrays[0] = startrays;

  for (unsigned int d = 1; d < depth; ++d) {
    vector<Ray2D> createdRays;

    for (auto &ray : allrays[d - 1]) {
			
			if (ray.terminated)
				continue;

      map<float,
          tuple<shared_ptr<Object2D>, IntersectResult2D>>
          intersections;

      for (auto &object : objects) {
        const auto result = object->intersect(ray);
        if (result.hit)
          intersections.insert({result.tEnter, {object, result}});
      }

      for (auto it = intersections.begin(); it != intersections.end(); it++) {
				auto &pair = *it;
				auto &[object, result] = pair.second;
				object->action(ray, result, createdRays);
				if(ray.terminated)
					break;
      }
    }
    allrays[d] = move(createdRays);
  }
  return allrays;
}
