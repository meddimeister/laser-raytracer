#include "scene.h"
#include "allmath.h"
#include "gtx/transform.hpp"
#include <iostream>
#include <map>
#include <vector>

void Scene2D::add(const shared_ptr<Object2D> &object) {
  objects.push_back(object);
}

void Scene2D::generatePointRays(const vec2 &origin, const vec2 &direction,
                                float maxAngle, float totalPower,
                                unsigned int count,
                                RNG::Sampler<float> &angleSampler,
                                RNG::ImportanceSampler1D &wavelengthSampler,
                                const function<float(float)> &spectrum) {

  angleSampler.init(count);
  wavelengthSampler.init(count);
  vector<Ray2D> generatedRays;
  float powerIntegral = 0.0f;
  for (unsigned int i = 0; i < count; ++i) {
    float angle = maxAngle * (2.0f * angleSampler.next() - 1.0f);
    vec2 dir = rotate(direction, angle);
    dir = normalize(dir);

    float wavelengthSample = wavelengthSampler.next();
    float wavelength = wavelengthSampler.value(wavelengthSample);
    float power =
        spectrum(wavelength) / wavelengthSampler.pdf(wavelengthSample);
    powerIntegral += power;

    generatedRays.push_back(Ray2D(origin, dir, power, wavelength));
  }
  for (auto &ray : generatedRays) {
    ray.power = ray.power * (totalPower / powerIntegral);
  }
  startrays.insert(startrays.end(), generatedRays.begin(), generatedRays.end());
}

void Scene2D::generateDirectionalRays(
    const vec2 &origin, float radius, const vec2 &direction, float totalPower,
    unsigned int count, RNG::Sampler<float> &originSampler,
    RNG::ImportanceSampler1D &wavelengthSampler,
    const function<float(float)> &spectrum) {

  originSampler.init(count);
  wavelengthSampler.init(count);
  vector<Ray2D> generatedRays;
  float powerIntegral = 0.0f;
  for (unsigned int i = 0; i < count; ++i) {
    float originRadius = radius * (2.0f * originSampler.next() - 1.0f);
    float wavelengthSample = wavelengthSampler.next();
    float wavelength = wavelengthSampler.value(wavelengthSample);
    float power =
        spectrum(wavelength) / wavelengthSampler.pdf(wavelengthSample);
    powerIntegral += power;

    generatedRays.push_back(
        Ray2D(origin + originRadius * rotate(direction, 0.5f * float(M_PI)),
              direction, power, wavelength));
  }
  for (auto &ray : generatedRays) {
    ray.power = ray.power * (totalPower / powerIntegral);
  }
  startrays.insert(startrays.end(), generatedRays.begin(), generatedRays.end());
}
vector<vector<Ray2D>> Scene2D::trace(unsigned int depth) {

  vector<vector<Ray2D>> allrays(depth);
  allrays[0] = startrays;

  for (unsigned int d = 1; d < depth; ++d) {
    vector<Ray2D> createdRays;

    for (auto &ray : allrays[d - 1]) {

      if (ray.terminated)
        continue;

      map<float, tuple<shared_ptr<Object2D>, IntersectResult2D>> intersections;

      for (auto &object : objects) {
        const auto result = object->intersect(ray);
        if (result.hit)
          intersections.insert({result.tEnter, {object, result}});
      }

      for (auto it = intersections.begin(); it != intersections.end(); it++) {
        auto &pair = *it;
        auto &[object, result] = pair.second;
        object->action(ray, result, createdRays);
        if (ray.terminated)
          break;
      }
    }
    allrays[d] = move(createdRays);
  }
  return allrays;
}
