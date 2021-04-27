#include "ray.h"
#include "random.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

	vector<Ray2D> rays = generatePointRays({0.0f,0.0f}, {0.0f, 1.0f}, 0.5f, 10, RNG::uniformLine);
  
	for(const auto& ray : rays){
		cout << ray << endl;
	}
}
