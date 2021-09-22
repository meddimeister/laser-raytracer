#pragma once

#include "allmath.h"
#include <tuple>

float snellius(float theta_e, float n_e, float n_t);

tuple<float, float, float, float> fresnel(float theta_e, float theta_t);
