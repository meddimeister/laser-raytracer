#include <vector>
#include <functional>
#include "glm.hpp"

using namespace std;
using namespace glm;

struct Ray {
	vec3 origin;
	vec3 direction;
};

vector<Ray> generatePointRays(vec3 origin, double angle, const function<vec3(void)> &&sampler);
