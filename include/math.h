#pragma once

#include "glm.hpp"

using namespace std;
using namespace glm;

inline float cross(const vec2 &a, const vec2 &b){
	return a.x * b.y - a.y * b.x;
}
