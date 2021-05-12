#pragma once

#include <random>
#include "gtc/random.hpp"
#include "glm.hpp"

using namespace std;
using namespace glm;

namespace RNG {
	float uniformLine();
	vec2 uniformDisk();
	vec3 uniformBall();
}
