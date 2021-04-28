#include "mirror.h"

vector<Ray2D> Mirror2D::reflect(vector<Ray2D> &rays){
	vector<Ray2D> reflections;
	for(auto &line : segments){
		auto ref = line.reflect(rays);
		reflections.insert(reflections.end(), ref.begin(), ref.end());
	}
	return reflections;
}
