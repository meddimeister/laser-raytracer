#pragma once

#include "object.h"
#include "random.h"

class Scene2D {
public:
  vector<shared_ptr<Object2D>> objects;
  vector<Ray2D> startrays;

  void add(const shared_ptr<Object2D> &object);

  void generatePointRays(const vec2 &origin, const vec2 &direction,
                         float maxAngle, float totalPower, unsigned int count,
                         RNG::Sampler<float> &angleSampler,
                         RNG::ImportanceSampler1D &wavelengthSampler,
                         const function<float(float)> &spectrum);

  void generateDirectionalRays(const vec2 &origin, float radius,
                               const vec2 &direction, float totalPower,
                               unsigned int count,
                               RNG::Sampler<float> &originSampler,
                               RNG::ImportanceSampler1D &wavelengthSampler,
                               const function<float(float)> &spectrum);

  vector<vector<Ray2D>> trace(unsigned int depth = 1);
};
