#include "scene.h"
#include "allmath.h"
#include "gtx/transform.hpp"
#include <cmath>
#include <iostream>
#include <map>
#include <vector>

void Scene2D::add(const shared_ptr<Object2D> &object) {
  objects.push_back(object);
}

void Scene2D::generatePointRays(const dvec2 &origin, const dvec2 &direction,
                                double maxAngle, double totalPower,
                                unsigned int count,
                                RNG::Sampler<double> &angleSampler,
                                RNG::ImportanceSampler1D &wavelengthSampler,
                                const function<double(double)> &spectrum) {

  angleSampler.init(count);
  wavelengthSampler.init(count);
  vector<Ray2D> generatedRays;
  double powerIntegral = 0.0;
  for (unsigned int i = 0; i < count; ++i) {
    double angle = maxAngle * (2.0 * angleSampler.next() - 1.0);
    dvec2 dir = rotate(direction, angle);
    dir = normalize(dir);

    double wavelengthSample = wavelengthSampler.next();
    double wavelength = wavelengthSampler.value(wavelengthSample);
    double power =
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
    const dvec2 &origin, double radius, const dvec2 &direction, double divergenceAngle, double totalPower,
    unsigned int count, RNG::Sampler<double> &originSampler, RNG::Sampler<double> &divergenceSampler,
    RNG::ImportanceSampler1D &wavelengthSampler,
    const function<double(double)> &spectrum) {

  originSampler.init(count);
  divergenceSampler.init(count);
  wavelengthSampler.init(count);
  vector<Ray2D> generatedRays;
  double powerIntegral = 0.0;
  for (unsigned int i = 0; i < count; ++i) {
    double originRadius = radius * (2.0 * originSampler.next() - 1.0);
    double divergence = 0.5 * divergenceAngle * (2.0 * divergenceSampler.next() - 1.0);
    double wavelengthSample = wavelengthSampler.next();
    double wavelength = wavelengthSampler.value(wavelengthSample);
    double power =
        spectrum(wavelength) / wavelengthSampler.pdf(wavelengthSample);
    if(isnan(power) || isinf(power))
      continue;
    powerIntegral += power;

    generatedRays.push_back(
        Ray2D(origin + originRadius * rotate(direction, 0.5 * double(M_PI)),
              rotate(direction, divergence), power, wavelength));
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

      map<double, tuple<shared_ptr<Object2D>, IntersectResult2D>> intersections;

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
