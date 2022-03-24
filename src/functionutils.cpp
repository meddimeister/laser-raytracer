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

function<double(double)> getFunction(vector<tuple<double, double>> &points,
                                     bool continueValues, bool debug) {
  if (debug) {
    CSVWriter csvWriter("csvOut");
    for (const auto &p : points) {
      csvWriter.add("test", get<0>(p), get<1>(p));
    }
    csvWriter.write();
  }
  sort(points.begin(), points.end(),
       [](tuple<double, double> &a, tuple<double, double> &b) {
         return get<0>(a) < get<0>(b);
       });
  return [points, continueValues](double x) {
    tuple<double, double> dummy(x, 0.0);
    auto it = lower_bound(
        points.begin(), points.end(), dummy,
        [&](const tuple<double, double> &a, const tuple<double, double> &b) {
          return get<0>(a) < get<0>(b);
        });
    if (it != points.end() && it != points.begin()) {
      auto [x_0, f_0] = *(it - 1);
      auto [x_1, f_1] = *(it);
      double alpha = (x - x_1) / (x_0 - x_1);
      return (1.0 - alpha) * f_0 + alpha * f_1;
    } else {
      if (continueValues) {
        if (it == points.begin()) {
          auto [x, f] = *(it);
          return f;
        } else {
          auto [x, f] = *(it - 1);
          return f;
        }
      } else {
        return 0.0;
      }
    }
  };
}

double integrateFunction(const function<double(double)> &f, double xMin,
                         double xMax, bool debug, size_t N) {
  double sum = 0.0;
  double dx = (xMax - xMin) / N;
  for (size_t i = 0; i < N; ++i) {
    double x = xMin + i * dx;
    if (debug) {
      if (!isinf(sum))
        cout << x << " " << f(x) << " " << sum << endl;
    }
    sum += f(x) * dx;
  }
  return sum;
}

function<double(double)> normalizeFunction(const function<double(double)> &f,
                                           double xMin, double xMax) {
  double integral = integrateFunction(f, xMin, xMax);
  return [f, xMin, xMax, integral](double x) {
    if (x < xMin || x > xMax) {
      return 0.0;
    } else {
      return f(x) / integral;
    }
  };
}

function<double(double)> get01Function(const function<double(double)> &f,
                                       double xMin, double xMax) {
  return [f, xMin, xMax](double x) {
    if (x < 0.0 || x > 1.0) {
      return 0.0;
    } else {
      return f(xMin + x * (xMax - xMin));
    }
  };
}

function<double(double)> getPdfFunction(const function<double(double)> &f,
                                        double xMin, double xMax) {
  return normalizeFunction(get01Function(f, xMin, xMax), 0.0, 1.0);
}

function<double(double)> getCdfFunction(const function<double(double)> &f,
                                        double xMin, double xMax, bool debug,
                                        size_t N) {
  auto pdf = getPdfFunction(f, xMin, xMax);
  vector<tuple<double, double>> cdf;
  double sum = 0.0;
  for (size_t i = 0; i < N; ++i) {
    cdf.push_back({double(i) / (N + 1), sum});
    double x = double(i) / N;
    if (debug && isnan(pdf(x))) {
      cout << x << endl;
      continue;
    }
    sum += pdf(x);
  }
  cdf.push_back({1.0, sum});
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

function<double(double)> scaleFunction(const function<double(double)> &f,
                                       double xFactor, double yFactor) {
  return [f, xFactor, yFactor](double x) { return yFactor * f(xFactor * x); };
}

function<double(double)> translateFunction(const function<double(double)> &f,
                                           double xOffset, double yOffset) {
  return [f, xOffset, yOffset](double x) { return yOffset + f(xOffset + x); };
}