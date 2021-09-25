#pragma once

#include <functional>
#include <vector>
#include <iostream>

using namespace std;

bool iterate(int dim, int n, const vector<int> &steps, vector<int> &idx, function<bool(const vector<int> &)> action)
{
    if(dim >= n){
        return action(idx);
    }
    for(int i = 0; i < steps[dim]; ++i){
        idx[dim] = i;
        bool cont = iterate(dim+1, n, steps, idx, action);
        if(!cont)
            return false;
    }
    return true;
}

void gridSearch(function<float(const vector<float> &)> f, const vector<float> &x,
                         const vector<int> &xSteps, const vector<float> &xDeltas)
{
    float f_x = f(x);
    vector<int> idx(x.size());
    iterate(0, x.size(), xSteps, idx, [&](const vector<int> &idx){
        vector<float> xSearchpoint = x;
        for(int dim = 0; dim < x.size(); ++dim){
            xSearchpoint[dim] += idx[dim] * xDeltas[dim];
            cout << xSearchpoint[dim] << " ";
        }
        float f_search = f(xSearchpoint);
        cout << f_search << endl;
        return true;
    });
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