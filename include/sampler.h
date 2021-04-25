#include "glm.hpp"
#include "gtc/random.hpp" 

using namespace glm;

inline vec2 diskSampler(unsigned int idx, unsigned int count){
	return diskRand(1.0f);
}
