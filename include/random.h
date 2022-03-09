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
  uniform_real_distribution<float> uniformDistribution;

public:
  void init(unsigned int _count) {
    this->count = _count;
    this->idx = 0;
    seed = chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);
    uniformDistribution = uniform_real_distribution<float>(0.0f, 1.0f);
  }

  virtual T next() = 0;
};

template <typename T> class NormalSampler : public Sampler<T> {
protected:
  unsigned int seed;
  default_random_engine generator;
  normal_distribution<float> normalDistribution;

public:
  void init(unsigned int _count) {
    this->count = _count;
    this->idx = 0;
    seed = chrono::system_clock::now().time_since_epoch().count();
    generator.seed(seed);
    normalDistribution = normal_distribution<float>(0.0f, 1.0f);
  }

  virtual T next() = 0;
};

class UniformSampler1D : public UniformSampler<float> {
public:
  float next() {
    float sample = this->uniformDistribution(this->generator);
    return sample;
  }
};

class StratifiedSampler1D : public UniformSampler<float> {
public:
  float next() {
    float sample = this->uniformDistribution(this->generator);
    return (idx++ + sample) / count;
  }
};

class NormalSampler1D : public NormalSampler<float> {
public:
  float next() {
    float sample = this->normalDistribution(this->generator);
    return sample;
  }
};

class ImportanceSampler1D : public UniformSampler<float> {
public:
  function<float(float)> _f;
  function<float(float)> _pdf;
  float _xMin;
  float _xMax;
  size_t _N;
  vector<float> _cdf;
  ImportanceSampler1D(function<float(float)> f, float xMin, float xMax, size_t N = 10000) : _f(f),
   _pdf(getPdfFunction(f, xMin, xMax)), _xMin(xMin), _xMax(xMax), _N(N){
    float sum = 0.0f;
    for(size_t i = 0; i < _N; ++i){
      _cdf.push_back(sum);
      float x = float(i)/_N;
      sum += _pdf(x);
    }
    _cdf.push_back(sum);
    for(size_t i = 0; i < _N + 1; ++i){
      _cdf[i] /= sum;
    }
  }  

  float next() {
    float sample = this->uniformDistribution(this->generator);

    float impSample = 0.f;
    for (size_t i = 1; i < _cdf.size(); i++) {
        if (sample < _cdf[i]) {
            float min = _cdf[i - 1];
            float max = _cdf[i];
            float alpha = (sample - min) / (max - min);
            impSample = (i - 1.f + alpha) / ((float) _N);
            break;
        }
    }
    return impSample;
  }

  float value(float sample){
    return _xMin + sample * (_xMax - _xMin);
  }

  float pdf(float sample){
    return _pdf(sample);
  }

  float f(float sample){
    return _f(value(sample));
  }
};

template <size_t N> class UniformSamplerND : public Sampler<vecn<float, N>> {
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

  vecn<float, N> next() {
    vecn<float, N> sampleVec;
    for (size_t i = 0; i < N; ++i) {
      sampleVec[i] = samplers[i].next();
    }
    return sampleVec;
  }
};

template <size_t N> class NormalSamplerND : public Sampler<vecn<float, N>> {
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

  vecn<float, N> next() {
    vecn<float, N> sampleVec;
    for (size_t i = 0; i < N; ++i) {
      sampleVec[i] = samplers[i].next();
    }
    return sampleVec;
  }
};

template <size_t N> class UniformBallSampler : public Sampler<vecn<float, N>> {
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

  vecn<float, N> next() {
    auto u = normalSampler.next();
    float norm = length(u);
    float r = pow(uniformSampler.next(), 1.0f / N);

    return (u * r) / norm;
  }
};
} // namespace RNG
