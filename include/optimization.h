#pragma once

#include "csv.h"
#include "funcan.h"
#include "iterateshapes.h"
#include "log.h"
#include "nomadbinding3.h"
#include "random.h"
#include "vecn.h"
#include <cstddef>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;

template <size_t N>
vecn<double, N> sequentialGridSearch(function<double(const vecn<double, N> &)> f,
                                    const vecn<double, N> &x,
                                    const vecn<int, N> &xSteps,
                                    const vecn<double, N> &xDeltas,
                                    bool checkAllPoints = true) {
  double min = f(x);
  vecn<int, N> idx;
  vecn<double, N> xMin = x;
  iterateSequentialGrid<N>(idx, xSteps, [&](const vecn<int, N> &idx) {
    auto xSearchpoint = x + xDeltas * idx;
    double f_search = f(xSearchpoint);
    if (f_search < min) {
      min = f_search;
      xMin = move(xSearchpoint);
      if (!checkAllPoints)
        return false;
    }
    return true;
  });
  return xMin;
}

template <size_t N>
vecn<double, N>
gridSearch(function<double(const vecn<double, N> &)> f, const vecn<double, N> &x,
           int radius, const vecn<double, N> &xDeltas,
           bool checkAllPoints = true, bool progressiveSteps = false) {
  double min = f(x);
  vecn<double, N> xMin = x;
  iterateGrid<N>(x.size(), radius, [&](const vecn<int, N> &idx) {
    auto xSearchpoint = x + xDeltas * idx;
    double f_search = f(xSearchpoint);
    if (f_search < min) {
      min = f_search;
      xMin = move(xSearchpoint);
      if (!checkAllPoints)
        return false;
    }
    return true;
  });
  return xMin;
}

template <size_t N>
vecn<double, N>
starSearch(function<double(const vecn<double, N> &)> f, const vecn<double, N> &x,
           int radius, const vecn<double, N> &xDeltas,
           bool checkAllPoints = true, bool progressiveSteps = false) {
  double min = f(x);
  auto xMin = x;
  iterateStar<N>(x.size(), radius, [&](const vecn<int, N> &idx) {
    auto xSearchpoint = x + xDeltas * idx;
    double f_search = f(xSearchpoint);
    if (f_search < min) {
      min = f_search;
      xMin = move(xSearchpoint);
      if (!checkAllPoints)
        return false;
    }
    return true;
  });
  return xMin;
}

template <size_t N>
vecn<double, N> ballSearch(function<double(const vecn<double, N> &)> f,
                          const vecn<double, N> &x, double radius, size_t count,
                          bool checkAllPoints = true) {
  RNG::UniformBallSampler<N> sampler;
  sampler.init(count);

  double min = f(x);
  auto xMin = x;

  for (size_t i = 0; i < count; ++i) {
    auto sample = sampler.next();
    auto xSearchpoint = x + sample * radius;
    double f_search = f(xSearchpoint);
    if (f_search < min) {
      min = f_search;
      xMin = move(xSearchpoint);
      if (!checkAllPoints)
        return xMin;
    }
  }

  return xMin;
}

template <size_t N>
vector<vecn<double, N>>
gradientDescent(function<double(const vecn<double, N> &)> f,
                const vecn<double, N> &xStart, int radius,
                const vecn<double, N> &deltas) {
  auto x = gridSearch<N>(f, xStart, radius, deltas);

  int iteration = 0;

  double lastf = f(x);
  while (1) {
    if (iteration++ > 1000)
      break;
    auto grad = gradientSecondOrder(f, x);

    DEBUG("CALCULATE GRADIENT");
    DEBUG("grad f(x) = " + to_string(grad[0]) + " " + to_string(grad[1]) +
          " f(x) = " + to_string(lastf));
    double L2 = length(grad);
    DEBUG("L2 = " + to_string(L2));

    // TODO: find optimal t with lineSearch
    // temporary hack
    DEBUG("FIND OPTIMAL STEPSIZE");
    auto x_t = x;
    double t_step = numeric_limits<double>::epsilon();
    int step = 1;
    double lastf_t = lastf;
    while (1) {
      for (int i = 0; i < x.size(); ++i) {
        x_t[i] = x[i] - step * t_step * grad[i];
      }
      double currentf_t = f(x_t);
      if (currentf_t >= lastf_t) {
        DEBUG("F GETTING BIGGER. STOP. f = " + to_string(currentf_t));
        step--;
        break;
      } else {
        DEBUG("F GETTING SMALLER f = " + to_string(currentf_t));
        step++;
      }
      lastf_t = currentf_t;
    }

    if (step != 0) {
      for (int i = 0; i < x.size(); ++i) {
        x[i] = x[i] - step * t_step * grad[i];
      }
      lastf = f(x);
    } else {
      DEBUG("NO CHANGE IN GRADIENT DIRECTION");
      auto xMinBall = ballSearch<N>(f, x, 0.1, 100);
      if (f(xMinBall) >= lastf) {
        DEBUG("MINIMUM FOUND");
        break;
      } else {
        DEBUG("SMALLER MINIMUM IN BALL");
        x = xMinBall;
      }
    }
  }

  return {x};
}

template <size_t N>
vector<vecn<double, N>>
mads(function<double(const vecn<double, N> &)> f1,function<double(const vecn<double, N> &)> f2, const vecn<double, N> &xStart,
     const vecn<double, N> &lowerBounds, const vecn<double, N> &upperBounds) {
  return runNomad<N>(f1, f2, xStart, lowerBounds, upperBounds);
}