#include <random>
#include "gtc/random.hpp"

using namespace std;
using namespace glm;

default_random_engine generator;
uniform_real_distribution<float> uniformDistribution(-1.0f, 1.0f);

namespace RNG {
	inline float uniformLine(){
		return uniformDistribution(generator);
	}

	inline vec2 uniformDisk(){
		return diskRand(1.0f);
	}

	inline vec3 uniformBall(){
		return ballRand(1.0f);
	}
}
