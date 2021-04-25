#include "ray.h"
#include "sampler.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

	vector<Ray> rays = generatePointRays({0.0f,0.0f,0.0f}, {0.0f, 0.0f, 1.0f}, 0.5f, 10, diskSampler);
  
	for(const auto& ray : rays){
		cout << ray << endl;
	}
}
