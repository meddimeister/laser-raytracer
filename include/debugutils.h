#pragma once
#include "random.h"

void outputHistogramm(RNG::ImportanceSampler1D &sampler,
                      const string &outputDir, const string &name,
                      size_t resolution = 10000, size_t N = 1000000);

void outputFunction(function<float(float)> f, float xMin, float xMax,
                    const string &outputDir, const string &name,
                    size_t resolution = 10000);