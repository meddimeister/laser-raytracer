#pragma once

#include "math/allmath.h"
#include <tuple>

double snellius(double theta_e, double n_e, double n_t);

tuple<double, double, double, double> fresnel(double theta_e, double theta_t);
