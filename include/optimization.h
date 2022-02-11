#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <iostream>
#include "csv.h"
#include "log.h"
#include "vecn.h"
#include "iterateshapes.h"
#include "funcan.h"
#include "random.h"

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
vecn<float, N> ballSearch(function<float(const vecn<float, N> &)> f, const vecn<float, N> &x,
                          float radius, size_t count, bool checkAllPoints = true){
    RNG::UniformBallSampler<N> sampler;
    sampler.init(count);

    float min = f(x);
    auto xMin = x;

    for(size_t i = 0; i < count; ++i){
        auto sample = sampler.next();
        auto xSearchpoint = x + sample * radius;
        float f_search = f(xSearchpoint);
        if (f_search < min)
        {
            min = f_search;
            xMin = move(xSearchpoint);
            if (!checkAllPoints)
                return xMin;
        }
    }

    return xMin;
}

template <size_t N>
vecn<float, N> optimize(function<float(const vecn<float, N> &)> f, const vecn<float, N> &xStart)
{
    auto x = xStart;

    int iteration = 0;

    float lastf = f(x);
    while (1)
    {
        if (iteration++ > 1000)
            break;
        auto grad = gradientSecondOrder(f, x);

        DEBUG("CALCULATE GRADIENT");
        DEBUG("grad f(x) = " + to_string(grad[0]) + " " + to_string(grad[1]) + " f(x) = " + to_string(lastf));
        float L2 = length(grad);
        DEBUG("L2 = " + to_string(L2));

        //TODO: find optimal t with lineSearch
        //temporary hack
        DEBUG("FIND OPTIMAL STEPSIZE");
        auto x_t = x;
        float t_step = numeric_limits<float>::epsilon();
        int step = 1;
        float lastf_t = lastf;
        while (1)
        {
            for (int i = 0; i < x.size(); ++i)
            {
                x_t[i] = x[i] - step * t_step * grad[i];
            }
            float currentf_t = f(x_t);
            if (currentf_t >= lastf_t)
            {
                DEBUG("F GETTING BIGGER. STOP. f = " + to_string(currentf_t));
                step--;
                break;
            }
            else
            {
                DEBUG("F GETTING SMALLER f = " + to_string(currentf_t));
                step++;
            }
            lastf_t = currentf_t;
        }

        if(step != 0){
            for (int i = 0; i < x.size(); ++i)
            {
                x[i] = x[i] - step * t_step * grad[i];
            }
            lastf = f(x);
        }
        else{
            DEBUG("NO CHANGE IN GRADIENT DIRECTION");
            auto xMinBall = ballSearch<N>(f, x, 0.1f, 100);
            if(f(xMinBall) >= lastf) {
                DEBUG("MINIMUM FOUND");
                break;
            }
            else {
                DEBUG("SMALLER MINIMUM IN BALL");
                x = xMinBall;
            }
        }
    }

    return x;
}