#pragma once

#include "glm.hpp"
#include "gtx/rotate_vector.hpp"
#include "gtx/vector_angle.hpp"

using namespace std;
using namespace glm;

inline float cross(const vec2 &a, const vec2 &b){
	return a.x * b.y - a.y * b.x;
}
