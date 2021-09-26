#pragma once

#include "optimization.h"
#include <iostream>
#include <limits>
#include <set>
#include <algorithm>

float normL2(const vector<float> &x)
{
    float L2 = 0.f;
    for (int i = 0; i < x.size(); ++i)
    {
        L2 += x[i] * x[i];
    }
    L2 = sqrt(L2);
    return L2;
}

vector<float> gradientFirstOrder(function<float(const vector<float> &)> f, const vector<float> &x,
                                 float h)
{
    vector<float> grad(x.size());
    float f_x = f(x);

    vector<float> xph = x;
    for (int i = 0; i < x.size(); ++i)
    {
        xph[i] += h;
        grad[i] = (f(xph) - f_x) / h;
        xph = x;
    }
    return grad;
}

bool iterateGrid(vector<int> &idx, const vector<int> &steps,
                 function<bool(const vector<int> &)> action, int dim = 0)
{
    if (dim >= idx.size())
    {
        return action(idx);
    }
    for (int i = 0; i < steps[dim]; ++i)
    {
        idx[dim] = i;
        bool cont = iterateGrid(idx, steps, action, dim + 1);
        if (!cont)
            return false;
    }
    return true;
}

void getConfigurations(
    int dim, vector<vector<int>> &configs, const vector<int> &config,
    function<bool(const vector<int> &)> reject = [](const vector<int> &c)
    { return false; },
    int covered = 0, int picked = 0)
{
    if (picked >= dim)
    {
        if (!reject(config))
            configs.push_back(config);
        return;
    }
    for (int i = covered; i < config.size(); ++i)
    {
        vector<int> newconfig = config;
        newconfig[i] += 1;
        getConfigurations(dim, configs, newconfig, reject, i, picked + 1);
    }
}

bool iterateProgressiveGrid(int dim, int radius, function<bool(const vector<int> &)> action,
                 int rad = 0)
{
    vector<int> allowed = {0};
    vector<int> configuration(dim);
    for (rad = 0; rad <= radius; ++rad)
    {
        //add new allowed index counts
        if (rad > 0)
        {
            allowed.push_back(rad);
            allowed.insert(allowed.begin(), -rad);
        }
        //build configuration
        vector<vector<int>> configs;
        vector<int> config(allowed.size(), 0);
        getConfigurations(dim, configs, config, [&](const vector<int> &c)
                          {
                              if (rad > 0)
                              {
                                  if (c[0] == 0 && c[c.size() - 1] == 0)
                                      return true;
                              }
                              return false;
                          });

        //permute configuration
        for (const auto &config : configs)
        {
            cout << "[ ";
            vector<int> permutation;
            for (int elementIdx = 0; elementIdx < config.size(); ++elementIdx)
            {
                cout << config[elementIdx] << " ";
                for(int i = 0; i < config[elementIdx]; ++i){
                    permutation.push_back(allowed[elementIdx]);
                }
            }
            cout << "] " << endl;
            do
            {
                for (const auto &idx : permutation)
                {
                    cout << idx << " ";
                }
                cout << endl;
            } while (next_permutation(permutation.begin(), permutation.end()));
        }
    }
    return true;
}

vector<float> gridSearch(function<float(const vector<float> &)> f, const vector<float> &x,
                         const vector<int> &xSteps, const vector<float> &xDeltas)
{
    float min = numeric_limits<float>::max();
    vector<int> idx(x.size());
    vector<float> xMin(x.size());
    iterateGrid(idx, xSteps, [&](const vector<int> &idx)
                {
                    vector<float> xSearchpoint = x;
                    for (int dim = 0; dim < x.size(); ++dim)
                    {
                        xSearchpoint[dim] += idx[dim] * xDeltas[dim];
                    }
                    float f_search = f(xSearchpoint);
                    if (f_search < min)
                    {
                        min = f_search;
                        xMin = move(xSearchpoint);
                    }
                    return true;
                });
    return xMin;
}

vector<float> progressiveGridSearch(function<float(const vector<float> &)> f, const vector<float> &x,
                         const vector<int> &xSteps, const vector<float> &xDeltas)
{
    iterateProgressiveGrid(2, 2, [&](const vector<int> &idx)
                { return true; });
    return {};
}

vector<float> gradientDescent(function<float(const vector<float> &)> f, const vector<float> &xStart)
{
    vector<float> x = xStart;

    float h = 0.01f;
    float epsilon = 0.000001f;
    float t = 0.01f;
    int iteration = 0;

    while (1)
    {
        if (iteration++ > 1000)
            break;
        vector<float> grad = gradientFirstOrder(f, x, h);

        cout << "grad " << grad[0] << " " << grad[1] << endl;
        float L2 = normL2(grad);

        if (L2 < epsilon)
        {
            //check if minimum in neighborhood
            //if not take minimum of neighborhood as new searchpoint
        }

        for (int i = 0; i < x.size(); ++i)
        {
            x[i] = x[i] - t * grad[i];
        }
    }

    return x;
}