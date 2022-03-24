#pragma once
#include "vecn.h"
#include <functional>
#include <tuple>
#include <vector>

function<double(double)> getFunction(vector<tuple<double, double>> &points,
                                     bool continueValues = false,
                                     bool debug = false);
double integrateFunction(const function<double(double)> &f, double xMin,
                         double xMax, bool debug = false, size_t N = 10000);
function<double(double)> normalizeFunction(const function<double(double)> &f,
                                           double xMin, double xMax);
function<double(double)> get01Function(const function<double(double)> &f,
                                       double xMin, double xMax);
function<double(double)> getPdfFunction(const function<double(double)> &f,
                                        double xMin, double xMax);
function<double(double)> getCdfFunction(const function<double(double)> &f,
                                        double xMin, double xMax,
                                        bool debug = false, size_t N = 10000);
function<double(double)> scaleFunction(const function<double(double)> &f,
                                       double xFactor, double yFactor = 1.0);
function<double(double)> translateFunction(const function<double(double)> &f,
                                           double xOffset,
                                           double yOffset = 0.0);
