#include <vector>
#include "glm.hpp"

using namespace std;

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};

//vector<Ray> generateRays(double origin[3], const function<vec3()> &&sampler);
