#include "debugutils.h"
#include "csv.h"
#include <cstddef>
#include <vector>

void outputHistogramm(RNG::ImportanceSampler1D &sampler,
                      const string &outputDir, const string &name,
                      size_t resolution, size_t N) {
  sampler.init(N);
  vector<int> histogramm(resolution, 0);

  for (size_t i = 0; i < N; ++i) {
    float sample = sampler.next();
    int idx = int(sample * resolution);
    histogramm[idx]++;
  }
  CSVWriter csvWriter(outputDir);
  csvWriter.add(name, "#idx", "count");

  for (size_t i = 0; i < resolution; ++i) {
    csvWriter.add(name, i, histogramm[i]);
  }

  csvWriter.write();
}

void outputFunction(function<float(float)> f, float xMin, float xMax, const string &outputDir,
                    const string &name, size_t resolution) {

  CSVWriter csvWriter(outputDir);
  csvWriter.add(name, "#x", "f(x)");

  for (size_t i = 0; i < resolution; ++i) {
    float x = xMin + (float(i)/resolution) * xMax;
   csvWriter.add(name, x, f(x));
  }

  csvWriter.write();
}