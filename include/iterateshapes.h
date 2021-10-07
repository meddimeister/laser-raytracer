#pragma once

#include <functional>
#include "vecn.h"

static void getConfigurations(
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

template<size_t N>
bool iterateSequentialGrid(vecn<int, N> &idx, const vecn<int, N> &steps,
                           function<bool(const vecn<int, N> &)> action, int dim = 0)
{
    if (dim >= idx.size())
    {
        return action(idx);
    }
    for (int i = 0; i < steps[dim]; ++i)
    {
        idx[dim] = i;
        bool cont = iterateSequentialGrid(idx, steps, action, dim + 1);
        if (!cont)
            return false;
    }
    return true;
}

template<size_t N>
bool iterateGrid(int dim, int radius, function<bool(const vecn<int, N> &)> action,
                 int rad = 0)
{
    vector<int> allowed = {0};
    vecn<int, N> configuration;
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
        vector<int> config(2, 0);
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
            vecn<int, N> permutation;
            int p = 0;
            for (int elementIdx = 0; elementIdx < config.size(); ++elementIdx)
            {
                for (int i = 0; i < config[elementIdx]; ++i)
                {
                    permutation[p++] = allowed[elementIdx];
                }
            }
            do
            {
                if (!action(permutation))
                    return false;
            } while (next_permutation(permutation.begin(), permutation.end()));
        }
    }
    return true;
}

template<size_t N>
bool iterateStar(int dim, int radius, function<bool(const vecn<int, N> &)> action,
                 int rad = 0)
{

    vecn<int, N> idx;
    idx.fill(0);
    if (!action(idx))
        return false;
    for (rad = 1; rad <= radius; ++rad)
    {
        for (int i = 0; i < dim; ++i)
        {
            idx[i] = rad;
            if (!action(idx))
                return false;
            fill(idx.begin(), idx.end(), 0);
            idx[i] = -rad;
            if (!action(idx))
                return false;
            fill(idx.begin(), idx.end(), 0);
        }
        vecn<int,2> allowed = {-rad, rad};
        vector<vecn<int, 2>> configs;
        vecn<int, 2> config;
        config.fill(0);
        getConfigurations(dim, configs, config);

        //permute configuration
        for (const auto &config : configs)
        {
            vecn<int, N> permutation;
            int p = 0;
            for (int elementIdx = 0; elementIdx < config.size(); ++elementIdx)
            {
                for (int i = 0; i < config[elementIdx]; ++i)
                {
                    permutation[p++] = allowed[elementIdx];
                }
            }
            do
            {
                if (!action(permutation))
                    return false;
            } while (next_permutation(permutation.begin(), permutation.end()));
        }
    }

    return true;
}