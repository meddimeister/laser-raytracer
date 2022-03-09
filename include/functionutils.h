#pragma once
#include "vecn.h"
#include <functional>
#include <tuple>
#include <vector>

function<float(float)> getFunction(vector<tuple<float, float>> &points,
                                   bool debug = false);
float integrateFunction(const function<float(float)> &f, float xMin, float xMax,
                        bool debug = false, size_t N = 10000);
function<float(float)> normalizeFunction(const function<float(float)> &f,
                                         float xMin, float xMax);
function<float(float)> get01Function(const function<float(float)> &f,
                                     float xMin, float xMax);
function<float(float)> getPdfFunction(const function<float(float)> &f,
                                      float xMin, float xMax);
function<float(float)> getCdfFunction(const function<float(float)> &f,
                                      float xMin, float xMax,
                                      bool debug = false, size_t N = 10000);
