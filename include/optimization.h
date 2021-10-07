#pragma once

#include <functional>
#include <vector>
#include <iostream>
#include "vecn.h"
#include "iterateshapes.h"
#include "funcan.h"

using namespace std;

template <size_t N>
vecn<float, N> sequentialGridSearch(function<float(const vecn<float, N> &)> f, const vecn<float, N> &x,
                                    const vecn<int, N> &xSteps, const vecn<float, N> &xDeltas,
                                    bool checkAllPoints = true)
{
    float min = f(x);
    vector<int> idx(x.size());
    vector<float> xMin = x;
    iterateSequentialGrid<N>(idx, xSteps, [&](const vecn<int, N> &idx)
                          {
                              auto xSearchpoint = x;
                              for (int dim = 0; dim < x.size(); ++dim)
                              {
                                  xSearchpoint[dim] += idx[dim] * xDeltas[dim];
                              }
                              float f_search = f(xSearchpoint);
                              if (f_search < min)
                              {
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
vecn<float, N> gridSearch(function<float(const vecn<float, N> &)> f, const vecn<float, N> &x,
                          int radius, const vecn<float, N> &xDeltas, bool checkAllPoints = true,
                          bool progressiveSteps = false)
{
    float min = f(x);
    auto xMin = x;
    iterateGrid<N>(x.size(), radius, [&](const vecn<int, N> &idx)
                {
                    auto xSearchpoint = x;
                    for (int dim = 0; dim < x.size(); ++dim)
                    {
                        xSearchpoint[dim] += idx[dim] * xDeltas[dim];
                    }
                    float f_search = f(xSearchpoint);
                    if (f_search < min)
                    {
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
vecn<float, N> starSearch(function<float(const vecn<float, N> &)> f, const vecn<float, N> &x,
                          int radius, const vecn<float, N> &xDeltas, bool checkAllPoints = true,
                          bool progressiveSteps = false)
{
    float min = f(x);
    auto xMin = x;
    iterateStar<N>(x.size(), radius, [&](const vecn<int, N> &idx)
                {
                    auto xSearchpoint = x;
                    for (int dim = 0; dim < x.size(); ++dim)
                    {
                        xSearchpoint[dim] += idx[dim] * xDeltas[dim];
                    }
                    float f_search = f(xSearchpoint);
                    if (f_search < min)
                    {
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
vecn<float, N> gradientDescent(function<float(const vecn<float, N> &)> f, const vecn<float, N> &xStart)
{
    auto x = xStart;

    float h = 0.0001f;
    float epsilon = 0.000001f;
    float t = 0.0001f;
    int iteration = 0;

    float lastf = f(x);
    while (1)
    {
        if (iteration++ > 1000)
            break;
        auto grad = gradientSecondOrder(f, x, h);

        cout << "grad f(x) = " << grad[0] << " " << grad[1] << " f(x) = " << lastf << endl;
        float L2 = length(grad);

        if (L2 < epsilon)
        {
            break;
            //TODO: check if minimum in neighborhood
            //TODO: if not take minimum of neighborhood as new searchpoint
        }

        //TODO: find optimal t with lineSearch
        //temporary hack
        auto x_t = x;
        float t_opt = 0.0001f;
        float lastf_t = lastf;
        while (1)
        {
            for (int i = 0; i < x.size(); ++i)
            {
                x_t[i] = x[i] - t_opt * grad[i];
            }
            float currentf_t = f(x_t);
            if (currentf_t > lastf_t)
            {
                t_opt -= 0.0001f;
                break;
            }
            else
            {
                t_opt += 0.0001f;
            }
            cout << currentf_t << endl;
            lastf_t = currentf_t;
        }

        for (int i = 0; i < x.size(); ++i)
        {
            x[i] = x[i] - t_opt * grad[i];
        }

        lastf = f(x);
    }

    return x;
}