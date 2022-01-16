#pragma once

#include <functional>
#include <limits>
#include "vecn.h"

template<size_t N>
vecn<float,N> gradientFirstOrder(function<float(const vecn<float, N> &)> f, const vecn<float, N> &x)
{
    vecn<float, N> grad;
    float f_x = f(x);

    vecn<float, N> xph = x;
    for (int i = 0; i < x.size(); ++i)
    {
        float h = sqrt(numeric_limits<float>::epsilon())*(x[i] + numeric_limits<float>::epsilon());
        volatile float xph_i = x[i] + h;
        float dx = xph_i - x[i];
        xph[i] = xph_i;
        grad[i] = (f(xph) - f_x) / dx;
        xph = x;
    }
    return grad;
}

template<size_t N>
vecn<float, N> gradientSecondOrder(function<float(const vecn<float, N> &)> f, const vecn<float, N> &x)
{
    vecn<float, N> grad;

    vecn<float, N> xph = x;
    vecn<float, N> xmh = x;
    for (int i = 0; i < x.size(); ++i)
    {    
        float h = cbrt(numeric_limits<float>::epsilon())*(x[i] + numeric_limits<float>::epsilon());
        volatile float xph_i = x[i] + h;
        volatile float xmh_i = x[i] - h;
        float dx = xph_i - xmh_i;
        DEBUG("dx = " + to_string(h));
        xph[i] = xph_i;
        xmh[i] = xmh_i;
        grad[i] = (f(xph) - f(xmh)) / dx;
        xph = x;
        xmh = x;
    }
    return grad;
}