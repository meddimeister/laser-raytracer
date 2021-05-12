#include "random.h"
#include <chrono>

unsigned int seed = chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);
uniform_real_distribution<float> uniformDistribution(-1.0f, 1.0f);

namespace RNG {
	float uniformLine(){
		return uniformDistribution(generator);
	}

	vec2 uniformDisk(){
		return diskRand(1.0f);
	}

	vec3 uniformBall(){
		return ballRand(1.0f);
	}
}
