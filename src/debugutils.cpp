#include "debugutils.h"
#include "csv.h"
#include <vector>
#include <cstddef>

void outputHistogramm(RNG::ImportanceSampler1D &sampler, const string &outputDir,
                      const string &name, size_t resolution, size_t N) {
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