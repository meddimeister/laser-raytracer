#pragma once

#include "math/allmath.h"
#include "types/vecn.h"
#include <tuple>

double snellius(double theta_e, double n_e, double n_t);

tuple<double, double, double, double> fresnel(double theta_e, double theta_t);

double sellmeier(const vecn<double, 4> &coeff, double wavelength);