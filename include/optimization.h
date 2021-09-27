#pragma once

#include <functional>
#include <vector>
#include <iostream>

using namespace std;

vector<float> sequentialGridSearch(function<float(const vector<float> &)> f, const vector<float> &x,
                                   const vector<int> &xSteps, const vector<float> &xDeltas,
                                   bool checkAllPoints = true);

vector<float> gridSearch(function<float(const vector<float> &)> f, const vector<float> &x,
                         int radius, const vector<float> &xDeltas, bool checkAllPoints = true,
                         bool progressiveSteps = false);

vector<float> starSearch(function<float(const vector<float> &)> f, const vector<float> &x,
                         int radius, const vector<float> &xDeltas, bool checkAllPoints = true,
                         bool progressiveSteps = false);

vector<float> gradientDescent(function<float(const vector<float> &)> f, const vector<float> &xStart);