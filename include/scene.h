#pragma once

#include "object.h"
#include "random.h"

class Scene2D {
public:
  vector<shared_ptr<Object2D>> objects;
  vector<Ray2D> startrays;

  void add(const shared_ptr<Object2D> &object);

  void generatePointRays(const dvec2 &origin, const dvec2 &direction,
                         double maxAngle, double totalPower, unsigned int count,
                         RNG::Sampler<double> &angleSampler,
                         RNG::ImportanceSampler1D &wavelengthSampler,
                         const function<double(double)> &spectrum);

  void generateDirectionalRays(const dvec2 &origin, double radius,
                               const dvec2 &direction, double divergenceAngle,
                               double totalPower, unsigned int count,
                               RNG::Sampler<double> &originSampler,
                               RNG::Sampler<double> &divergenceSampler,
                               RNG::ImportanceSampler1D &wavelengthSampler,
                               const function<double(double)> &spectrum);

  vector<vector<Ray2D>> trace(unsigned int depth = 1);
};
