#pragma once

#include <functional>
#include "vecn.h"

template<size_t N>
vecn<float,N> gradientFirstOrder(function<float(const vecn<float, N> &)> f, const vecn<float, N> &x,
                                 float h)
{
    vecn<float, N> grad;
    float f_x = f(x);

    vecn<float, N> xph = x;
    for (int i = 0; i < x.size(); ++i)
    {
        xph[i] += h;
        grad[i] = (f(xph) - f_x) / h;
        xph = x;
    }
    return grad;
}

template<size_t N>
vecn<float, N> gradientSecondOrder(function<float(const vecn<float, N> &)> f, const vecn<float, N> &x,
                                 float h)
{
    vecn<float, N> grad;

    vecn<float, N> xph = x;
    vecn<float, N> xmh = x;
    for (int i = 0; i < x.size(); ++i)
    {
        xph[i] += h;
        xmh[i] -= h;
        grad[i] = (f(xph) - f(xmh)) / (2*h);
        xph = x;
        xmh = x;
    }
    return grad;
}