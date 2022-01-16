#pragma once

#include <functional>
#include <vector>
#include <iostream>
#include "log.h"
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
    vecn<int, N> idx;
    vecn<float, N> xMin = x;
    iterateSequentialGrid<N>(idx, xSteps, [&](const vecn<int, N> &idx)
                             {
                                 auto xSearchpoint = x + xDeltas * idx;
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
    vecn<float, N> xMin = x;
    iterateGrid<N>(x.size(), radius, [&](const vecn<int, N> &idx)
                   {
                       auto xSearchpoint = x + xDeltas * idx;
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
                       auto xSearchpoint = x + xDeltas * idx;
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

    //TODO: choose adaptive slope parameter h depending on x and machine error --> error analysis explained in paper
    float h = 0.0001f;

    //TODO: how to choose epsilon? Error analysis?
    float epsilon = 0.000001f;
    int iteration = 0;

    float lastf = f(x);
    while (1)
    {
        if (iteration++ > 1000)
            break;
        auto grad = gradientSecondOrder(f, x, h);

        DEBUG("grad f(x) = " + to_string(grad[0]) + " " + to_string(grad[1]) + " f(x) = " + to_string(lastf));
        float L2 = length(grad);

        if (L2 < epsilon)
        {
            break;
            //TODO: check if current x is minimum in ball neighborhood
            //if not take minimum of neighborhood as new searchpoint
            //if yes assume we have local minimum --> return x
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
            DEBUG(to_string(currentf_t));
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