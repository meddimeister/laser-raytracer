#include "functionutils.h"
#include "csv.h"
#include "vecn.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

function<float(float)> getFunction(vector<tuple<float, float>> &points,
                                   bool debug) {
  if (debug) {
    CSVWriter csvWriter("csvOut");
    for (const auto &p : points) {
      csvWriter.add("test", get<0>(p), get<1>(p));
    }
    csvWriter.write();
  }
  sort(points.begin(), points.end(),
       [](tuple<float, float> &a, tuple<float, float> &b) {
         return get<0>(a) < get<0>(b);
       });
  return [points](float x) {
    tuple<float, float> dummy(x, 0.0f);
    auto it = lower_bound(
        points.begin(), points.end(), dummy,
        [&](const tuple<float, float> &a, const tuple<float, float> &b) { return get<0>(a) < get<0>(b);});
    if (it != points.end() && it != points.begin()) {
      auto [x_0, f_0] = *(it - 1);
      auto [x_1, f_1] = *(it);
      float alpha = (x - x_1) / (x_0 - x_1);
      return (1.0f - alpha) * f_0 + alpha * f_1;
    };
    return 0.0f;
  };
}

float integrateFunction(const function<float(float)> &f, float xMin, float xMax,
                        bool debug, size_t N) {
  float sum = 0.0f;
  float dx = (xMax - xMin) / N;
  for (size_t i = 0; i < N; ++i) {
    float x = xMin + i * dx;
    if (debug) {
      if (!isinf(sum))
        cout << x << " " << f(x) << " " << sum << endl;
    }
    sum += f(x) * dx;
  }
  return sum;
}

function<float(float)> normalizeFunction(const function<float(float)> &f,
                                         float xMin, float xMax) {
  float integral = integrateFunction(f, xMin, xMax);
  return [f, xMin, xMax, integral](float x) {
    if (x < xMin || x > xMax) {
      return 0.0f;
    } else {
      return f(x) / integral;
    }
  };
}

function<float(float)> get01Function(const function<float(float)> &f,
                                     float xMin, float xMax) {
  return [f, xMin, xMax](float x) {
    if (x < 0.0f || x > 1.0f) {
      return 0.0f;
    } else {
      return f(xMin + x * (xMax - xMin));
    }
  };
}

function<float(float)> getPdfFunction(const function<float(float)> &f,
                                      float xMin, float xMax) {
  return normalizeFunction(get01Function(f, xMin, xMax), 0.0f, 1.0f);
}

function<float(float)> getCdfFunction(const function<float(float)> &f,
                                      float xMin, float xMax, bool debug,
                                      size_t N) {
  auto pdf = getPdfFunction(f, xMin, xMax);
  vector<tuple<float, float>> cdf;
  float sum = 0.0f;
  for (size_t i = 0; i < N; ++i) {
    cdf.push_back({float(i) / (N + 1), sum});
    float x = float(i) / N;
    if (debug && isnan(pdf(x))) {
      cout << x << endl;
      continue;
    }
    sum += pdf(x);
  }
  cdf.push_back({1.0f, sum});
  for (size_t i = 0; i < N + 1; ++i) {
    get<1>(cdf[i]) /= sum;
  }
  if (debug) {
    CSVWriter csvWriter("csvOut");
    for (const auto &p : cdf) {
      csvWriter.add("test", get<0>(p), get<1>(p));
    }
    csvWriter.write();
  }
  return getFunction(cdf);
}