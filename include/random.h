#pragma once

#include "glm.hpp"
#include "gtc/random.hpp"
#include "vecn.h"
#include <array>
#include <chrono>
#include <cstddef>
#include <functional>
#include <random>
#include "functionutils.h"

using namespace std;
using namespace glm;

namespace RNG {
template <typename T> class Sampler {
protected:
  unsigned int count;
  unsigned int idx;

public:
  virtual void init(unsigned int count) = 0;
  virtual T next() = 0;
};

template <typename T> class UniformSampler : public Sampler<T> {
protected:
  unsigned int seed;
  default_random_engine generator;
  uniform_real_distribution<double> uniformDistribution;

public:
  void init(unsigned int _count) {
    this->count = _count;
    this->idx = 0;
    seed = chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);
    uniformDistribution = uniform_real_distribution<double>(0.0, 1.0);
  }

  virtual T next() = 0;
};

template <typename T> class NormalSampler : public Sampler<T> {
protected:
  unsigned int seed;
  default_random_engine generator;
  normal_distribution<double> normalDistribution;

public:
  void init(unsigned int _count) {
    this->count = _count;
    this->idx = 0;
    seed = chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);
    normalDistribution = normal_distribution<double>(0.0, 1.0);
  }

  virtual T next() = 0;
};

class UniformSampler1D : public UniformSampler<double> {
public:
  double next() {
    double sample = this->uniformDistribution(this->generator);
    return sample;
  }
};

class StratifiedSampler1D : public UniformSampler<double> {
public:
  double next() {
    double sample = this->uniformDistribution(this->generator);
    return (idx++ + sample) / count;
  }
};

class NormalSampler1D : public NormalSampler<double> {
public:
  double next() {
    double sample = this->normalDistribution(this->generator);
    return sample;
  }
};

class ImportanceSampler1D : public UniformSampler<double> {
public:
  function<double(double)> _f;
  function<double(double)> _pdf;
  double _xMin;
  double _xMax;
  size_t _N;
  vector<double> _cdf;
  ImportanceSampler1D(function<double(double)> f, double xMin, double xMax, size_t N = 10000) : _f(f),
   _pdf(getPdfFunction(f, xMin, xMax)), _xMin(xMin), _xMax(xMax), _N(N){
    double sum = 0.0;
    for(size_t i = 0; i < _N; ++i){
      _cdf.push_back(sum);
      double x = double(i)/_N;
      sum += _pdf(x);
    }
    _cdf.push_back(sum);
    for(size_t i = 0; i < _N + 1; ++i){
      _cdf[i] /= sum;
    }
  }  

  double next() {
    double sample = this->uniformDistribution(this->generator);

    double impSample = 0.;
    for (size_t i = 1; i < _cdf.size(); i++) {
        if (sample < _cdf[i]) {
            double min = _cdf[i - 1];
            double max = _cdf[i];
            double alpha = (sample - min) / (max - min);
            impSample = (i - 1. + alpha) / ((double) _N);
            break;
        }
    }
    return impSample;
  }

  double value(double sample){
    return _xMin + sample * (_xMax - _xMin);
  }

  double pdf(double sample){
    return _pdf(sample);
  }

  double f(double sample){
    return _f(value(sample));
  }
};

template <size_t N> class UniformSamplerND : public Sampler<vecn<double, N>> {
protected:
  array<UniformSampler1D, N> samplers;

public:
  void init(unsigned int _count) {
    this->idx = 0;
    this->count = _count;
    for (auto &sampler : samplers) {
      sampler.init(_count);
    }
  }

  vecn<double, N> next() {
    vecn<double, N> sampleVec;
    for (size_t i = 0; i < N; ++i) {
      sampleVec[i] = samplers[i].next();
    }
    return sampleVec;
  }
};

template <size_t N> class NormalSamplerND : public Sampler<vecn<double, N>> {
protected:
  array<NormalSampler1D, N> samplers;

public:
  void init(unsigned int _count) {
    this->idx = 0;
    this->count = _count;
    for (auto &sampler : samplers) {
      sampler.init(_count);
    }
  }

  vecn<double, N> next() {
    vecn<double, N> sampleVec;
    for (size_t i = 0; i < N; ++i) {
      sampleVec[i] = samplers[i].next();
    }
    return sampleVec;
  }
};

template <size_t N> class UniformBallSampler : public Sampler<vecn<double, N>> {
protected:
  NormalSamplerND<N> normalSampler;
  UniformSampler1D uniformSampler;

public:
  void init(unsigned int _count) {
    this->idx = 0;
    this->count = _count;
    normalSampler.init(_count);
    uniformSampler.init(_count);
  }

  vecn<double, N> next() {
    auto u = normalSampler.next();
    double norm = length(u);
    double r = pow(uniformSampler.next(), 1.0 / N);

    return (u * r) / norm;
  }
};
} // namespace RNG
