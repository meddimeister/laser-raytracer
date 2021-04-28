#include "ray.h"
#include "random.h"
#include "trace.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

	vector<Ray2D> rays = generatePointRays({0.0f,0.0f}, {0.0f, 1.0f}, 0.5f, 10, RNG::uniformLine);
  
	for(const auto& ray : rays){
		cout << ray << endl;
	}

	Ray2D testray({0.0f, 0.0f}, {1.0f, 0.0f});
	cout << testray << endl;

	TraceResult2D traceResult = intersectLine({0.0f, -1.0f}, {2.0f, 1.0f}, testray);
	cout << traceResult << endl;

	Ray2D reflection = reflect(testray, traceResult.t, traceResult.normal);
	cout << reflection << endl;
}
