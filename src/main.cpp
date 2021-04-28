#include "ray.h"
#include "random.h"
#include "shape.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

	vector<Ray2D> rays = generatePointRays({0.0f,0.0f}, {0.0f, 1.0f}, 0.5f, 10, RNG::uniformLine);
  
	for(const auto& ray : rays){
		cout << ray << endl;
	}

	Ray2D testray({0.0f, 0.0f}, {1.0f, 0.0f});
	cout << testray << endl;

	Line2D line({0.0f, -1.0f}, {2.0f, 1.0f});
	cout << line << endl; 

	IntersectResult2D intersectResult = line.intersect(testray);
	cout << intersectResult << endl;

	Ray2D reflection = testray.reflect(intersectResult.t, intersectResult.normal);
	cout << reflection << endl;
}
