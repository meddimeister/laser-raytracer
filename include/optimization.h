#pragma once

#include <functional>
#include <vector>
#include <iostream>

using namespace std;

void gradientDescent(function<float(const vector<float> &)> f, const vector<float> &xStart,
                     function<void(const vector<float>&)> postIteration)
{
    vector<float> x = xStart;
    vector<float> grad(x.size());
    float f_x = f(x);

    float h = 0.1f;
    float epsilon = 0.000001f;

    vector<float> xph = x;
    for (int i = 0; i < x.size(); ++i)
    {
        xph[i] += h;
        grad[i] = (f(xph) - f_x) / h;
        cout << f(xph) << " " << f_x << endl;
        xph = x;
    }

    float L2 = 0.f;

    for (int i = 0; i < x.size(); ++i)
    {
        L2 += grad[i] * grad[i];
    }

    float t = 0.01f;
    int iteration = 0;

    while (L2 > epsilon && iteration < 1000)
    {
        for (int i = 0; i < x.size(); ++i)
        {
            x[i] = x[i] - t * grad[i];
        }

        f_x = f(x);
        postIteration(x);

        vector<float> xph = x;
        for (int i = 0; i < x.size(); ++i)
        {
            xph[i] += h;
            grad[i] = (f(xph) - f_x)/h;
        }

        L2 = 0.f;

        for (int i = 0; i < x.size(); ++i)
        {
            L2 += grad[i] * grad[i];
        }

        ++iteration;
    }
}