#pragma once

#include <functional>
#include <vector>
#include <iostream>

using namespace std;

vector<float> gridSearch(function<float(const vector<float> &)> f, const vector<float> &x,
                const vector<int> &xSteps, const vector<float> &xDeltas);

void gradientDescent(function<float(const vector<float> &)> f, const vector<float> &xStart);