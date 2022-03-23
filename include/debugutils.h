#pragma once
#include "random.h"

void outputHistogramm(RNG::ImportanceSampler1D &sampler,
                      const string &outputDir, const string &name,
                      size_t resolution = 10000, size_t N = 1000000);

void outputFunction(function<double(double)> f, double xMin, double xMax,
                    const string &outputDir, const string &name,
                    size_t resolution = 10000);