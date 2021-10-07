#pragma once

#include "optimization.h"
#include <iostream>
#include <limits>
#include <set>
#include <algorithm>

//float normL2(const vector<float> &x)
//{
//    float L2 = 0.f;
//    for (int i = 0; i < x.size(); ++i)
//    {
//        L2 += x[i] * x[i];
//    }
//    L2 = sqrt(L2);
//    return L2;
//}

//vector<float> gradientFirstOrder(function<float(const vector<float> &)> f, const vector<float> &x,
//                                 float h)
//{
//    vector<float> grad(x.size());
//    float f_x = f(x);
//
//    vector<float> xph = x;
//    for (int i = 0; i < x.size(); ++i)
//    {
//        xph[i] += h;
//        grad[i] = (f(xph) - f_x) / h;
//        xph = x;
//    }
//    return grad;
//}
//
//vector<float> gradientSecondOrder(function<float(const vector<float> &)> f, const vector<float> &x,
//                                 float h)
//{
//    vector<float> grad(x.size());
//
//    vector<float> xph = x;
//    vector<float> xmh = x;
//    for (int i = 0; i < x.size(); ++i)
//    {
//        xph[i] += h;
//        xmh[i] -= h;
//        grad[i] = (f(xph) - f(xmh)) / (2*h);
//        xph = x;
//        xmh = x;
//    }
//    return grad;
//}

//bool iterateSequentialGrid(vector<int> &idx, const vector<int> &steps,
//                           function<bool(const vector<int> &)> action, int dim = 0)
//{
//    if (dim >= idx.size())
//    {
//        return action(idx);
//    }
//    for (int i = 0; i < steps[dim]; ++i)
//    {
//        idx[dim] = i;
//        bool cont = iterateSequentialGrid(idx, steps, action, dim + 1);
//        if (!cont)
//            return false;
//    }
//    return true;
//}
//
//void getConfigurations(
//    int dim, vector<vector<int>> &configs, const vector<int> &config,
//    function<bool(const vector<int> &)> reject = [](const vector<int> &c)
//    { return false; },
//    int covered = 0, int picked = 0)
//{
//    if (picked >= dim)
//    {
//        if (!reject(config))
//            configs.push_back(config);
//        return;
//    }
//    for (int i = covered; i < config.size(); ++i)
//    {
//        vector<int> newconfig = config;
//        newconfig[i] += 1;
//        getConfigurations(dim, configs, newconfig, reject, i, picked + 1);
//    }
//}
//
//bool iterateGrid(int dim, int radius, function<bool(const vector<int> &)> action,
//                 int rad = 0)
//{
//    vector<int> allowed = {0};
//    vector<int> configuration(dim);
//    for (rad = 0; rad <= radius; ++rad)
//    {
//        //add new allowed index counts
//        if (rad > 0)
//        {
//            allowed.push_back(rad);
//            allowed.insert(allowed.begin(), -rad);
//        }
//        //build configuration
//        vector<vector<int>> configs;
//        vector<int> config(allowed.size(), 0);
//        getConfigurations(dim, configs, config, [&](const vector<int> &c)
//                          {
//                              if (rad > 0)
//                              {
//                                  if (c[0] == 0 && c[c.size() - 1] == 0)
//                                      return true;
//                              }
//                              return false;
//                          });
//
//        //permute configuration
//        for (const auto &config : configs)
//        {
//            vector<int> permutation;
//            for (int elementIdx = 0; elementIdx < config.size(); ++elementIdx)
//            {
//                for (int i = 0; i < config[elementIdx]; ++i)
//                {
//                    permutation.push_back(allowed[elementIdx]);
//                }
//            }
//            do
//            {
//                if (!action(permutation))
//                    return false;
//            } while (next_permutation(permutation.begin(), permutation.end()));
//        }
//    }
//    return true;
//}
//
//bool iterateStar(int dim, int radius, function<bool(const vector<int> &)> action,
//                 int rad = 0)
//{
//
//    vector<int> idx(dim, 0);
//    if (!action(idx))
//        return false;
//    for (rad = 1; rad <= radius; ++rad)
//    {
//        for (int i = 0; i < dim; ++i)
//        {
//            idx[i] = rad;
//            if (!action(idx))
//                return false;
//            fill(idx.begin(), idx.end(), 0);
//            idx[i] = -rad;
//            if (!action(idx))
//                return false;
//            fill(idx.begin(), idx.end(), 0);
//        }
//        vector<int> allowed = {-rad, rad};
//        vector<vector<int>> configs;
//        vector<int> config(2, 0);
//        getConfigurations(dim, configs, config);
//
//        //permute configuration
//        for (const auto &config : configs)
//        {
//            vector<int> permutation;
//            for (int elementIdx = 0; elementIdx < config.size(); ++elementIdx)
//            {
//                for (int i = 0; i < config[elementIdx]; ++i)
//                {
//                    permutation.push_back(allowed[elementIdx]);
//                }
//            }
//            do
//            {
//                if (!action(permutation))
//                    return false;
//            } while (next_permutation(permutation.begin(), permutation.end()));
//        }
//    }
//
//    return true;
//}

//vector<float> sequentialGridSearch(function<float(const vector<float> &)> f, const vector<float> &x,
//                                   const vector<int> &xSteps, const vector<float> &xDeltas,
//                                   bool checkAllPoints)
//{
//    float min = f(x);
//    vector<int> idx(x.size());
//    vector<float> xMin = x;
//    iterateSequentialGrid(idx, xSteps, [&](const vector<int> &idx)
//                          {
//                              vector<float> xSearchpoint = x;
//                              for (int dim = 0; dim < x.size(); ++dim)
//                              {
//                                  xSearchpoint[dim] += idx[dim] * xDeltas[dim];
//                              }
//                              float f_search = f(xSearchpoint);
//                              if (f_search < min)
//                              {
//                                  min = f_search;
//                                  xMin = move(xSearchpoint);
//                                  if (!checkAllPoints)
//                                      return false;
//                              }
//                              return true;
//                          });
//    return xMin;
//}

//vector<float> gridSearch(function<float(const vector<float> &)> f, const vector<float> &x,
//                         int radius, const vector<float> &xDeltas, bool checkAllPoints,
//                         bool progressiveSteps)
//{
//    float min = f(x);
//    vector<float> xMin = x;
//    iterateGrid(x.size(), radius, [&](const vector<int> &idx)
//                {
//                    vector<float> xSearchpoint = x;
//                    for (int dim = 0; dim < x.size(); ++dim)
//                    {
//                        xSearchpoint[dim] += idx[dim] * xDeltas[dim];
//                    }
//                    float f_search = f(xSearchpoint);
//                    if (f_search < min)
//                    {
//                        min = f_search;
//                        xMin = move(xSearchpoint);
//                        if (!checkAllPoints)
//                            return false;
//                    }
//                    return true;
//                });
//    return xMin;
//}

//vector<float> starSearch(function<float(const vector<float> &)> f, const vector<float> &x,
//                         int radius, const vector<float> &xDeltas, bool checkAllPoints,
//                         bool progressiveSteps)
//{
//    float min = f(x);
//    vector<float> xMin = x;
//    iterateStar(x.size(), radius, [&](const vector<int> &idx)
//                {
//                    vector<float> xSearchpoint = x;
//                    for (int dim = 0; dim < x.size(); ++dim)
//                    {
//                        xSearchpoint[dim] += idx[dim] * xDeltas[dim];
//                    }
//                    float f_search = f(xSearchpoint);
//                    if (f_search < min)
//                    {
//                        min = f_search;
//                        xMin = move(xSearchpoint);
//                        if (!checkAllPoints)
//                            return false;
//                    }
//                    return true;
//                });
//    return xMin;
//}

//vector<float> gradientDescent(function<float(const vector<float> &)> f, const vector<float> &xStart)
//{
//    vector<float> x = xStart;
//
//    float h = 0.0001f;
//    float epsilon = 0.000001f;
//    float t = 0.0001f;
//    int iteration = 0;
//
//    float lastf = f(x);
//    while (1)
//    {
//        if (iteration++ > 1000)
//            break;
//        vector<float> grad = gradientSecondOrder(f, x, h);
//
//        cout << "grad f(x) = " << grad[0] << " " << grad[1] << " f(x) = " << lastf << endl;
//        float L2 = normL2(grad);
//
//        if (L2 < epsilon)
//        {
//            break;
//            //TODO: check if minimum in neighborhood
//            //TODO: if not take minimum of neighborhood as new searchpoint
//        }
//
//        //TODO: find optimal t with lineSearch
//        //temporary hack
//        vector<float> x_t = x;
//        float t_opt = 0.0001f;
//        float lastf_t = lastf;
//        while(1){
//            for (int i = 0; i < x.size(); ++i)
//            {
//                x_t[i] = x[i] - t_opt * grad[i];
//            }
//            float currentf_t = f(x_t);
//            if(currentf_t > lastf_t){
//                t_opt -= 0.0001f;
//                break;
//            }
//            else{
//                t_opt += 0.0001f;
//            }
//            cout << currentf_t << endl;
//            lastf_t = currentf_t;
//        }
//
//        for (int i = 0; i < x.size(); ++i)
//        {
//            x[i] = x[i] - t_opt * grad[i];
//        }
//
//        lastf = f(x);
//    }
//
//    return x;
//}