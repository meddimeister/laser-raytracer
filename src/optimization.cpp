#pragma once

#include "optimization.h"
#include <iostream>
#include <limits>

bool iterate(int n, const vector<int> &steps, function<bool(const vector<int> &)> action,
 vector<int> &idx, int dim = 0)
{
    if (dim >= n)
    {
        return action(idx);
    }
    for (int i = 0; i < steps[dim]; ++i)
    {
        idx[dim] = i;
        bool cont = iterate(n, steps, action, idx, dim + 1 );
        if (!cont)
            return false;
    }
    return true;
}

vector<float> gridSearch(function<float(const vector<float> &)> f, const vector<float> &x,
                const vector<int> &xSteps, const vector<float> &xDeltas)
{
    float min = numeric_limits<float>::max();
    vector<int> idx(x.size());
    vector<float> xMin(x.size());
    iterate(x.size(), xSteps, [&](const vector<int> &idx)
            {
                vector<float> xSearchpoint = x;
                for (int dim = 0; dim < x.size(); ++dim)
                {
                    xSearchpoint[dim] += idx[dim] * xDeltas[dim];
                }
                float f_search = f(xSearchpoint);
                if(f_search < min){
                    min = f_search;
                    xMin = move(xSearchpoint);
                }
                return true;
            }, idx);
    return xMin;
}

void gradientDescent(function<float(const vector<float> &)> f, const vector<float> &xStart)
{
    vector<float> x = xStart;
    vector<float> grad(x.size());
    float f_x = f(x);

    float h = 0.01f;
    float epsilon = 0.000001f;

    vector<float> xph = x;
    for (int i = 0; i < x.size(); ++i)
    {
        xph[i] += h;
        grad[i] = (f(xph) - f_x) / h;
        xph = x;
    }

    cout << "grad " << grad[0] << " " << grad[1] << endl;
    float L2 = 0.f;

    for (int i = 0; i < x.size(); ++i)
    {
        L2 += grad[i] * grad[i];
    }

    L2 = sqrt(L2);

    float t = 0.01f;
    int iteration = 0;

    while (L2 > epsilon && iteration < 1000)
    {
        for (int i = 0; i < x.size(); ++i)
        {
            x[i] = x[i] - t * grad[i];
        }

        f_x = f(x);

        vector<float> xph = x;
        for (int i = 0; i < x.size(); ++i)
        {
            xph[i] += h;
            grad[i] = (f(xph) - f_x) / h;
            xph = x;
        }

        cout << "grad " << grad[0] << " " << grad[1] << endl;
        L2 = 0.f;

        for (int i = 0; i < x.size(); ++i)
        {
            L2 += grad[i] * grad[i];
        }

        L2 = sqrt(L2);
        ++iteration;
    }
}