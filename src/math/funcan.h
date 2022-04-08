#pragma once

#include "types/vecn.h"
#include <cmath>
#include <functional>
#include <limits>

template <size_t N>
vecn<double, N> gradientFirstOrder(function<double(const vecn<double, N> &)> f,
                                  const vecn<double, N> &x) {
  vecn<double, N> grad;
  double f_x = f(x);

  vecn<double, N> xph = x;
  for (int i = 0; i < x.size(); ++i) {
    double h = sqrt(numeric_limits<double>::epsilon()) *
              (x[i] + numeric_limits<double>::epsilon());
    volatile double xph_i = x[i] + h;
    double dx = xph_i - x[i];
    xph[i] = xph_i;
    grad[i] = (f(xph) - f_x) / dx;
    xph = x;
  }
  return grad;
}

template <size_t N>
vecn<double, N> gradientSecondOrder(function<double(const vecn<double, N> &)> f,
                                   const vecn<double, N> &x) {
  vecn<double, N> grad;

  vecn<double, N> xph = x;
  vecn<double, N> xmh = x;
  for (int i = 0; i < x.size(); ++i) {
    double h = cbrt(numeric_limits<double>::epsilon()) *
              (x[i] + numeric_limits<double>::epsilon());
    volatile double xph_i = x[i] + h;
    volatile double xmh_i = x[i] - h;
    double dx = xph_i - xmh_i;
    xph[i] = xph_i;
    xmh[i] = xmh_i;
    grad[i] = (f(xph) - f(xmh)) / dx;
    xph = x;
    xmh = x;
  }
  return grad;
}