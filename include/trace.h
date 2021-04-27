#pragma once

#include "glm.hpp"
#include "ray.h"
#include <iostream>

using namespace std;
using namespace glm;

struct TraceResult {
	float t;
	bool hit = false;
};

inline TraceResult intersectLine(const vec2 &a , const vec2 &b, const Ray2D &ray){

	TraceResult result;

	float t = ((ray.origin.x - a.x)*(b.y - a.y) + (ray.origin.y - a.y)*(b.x - a.x))/
		(ray.direction.y*(b.x - a.x) - ray.direction.x*(b.y - a.y));
	
	if(t >= 0.0f){
		float alpha;

		if(b.x != a.x)
			alpha = (ray.origin.x + t * ray.direction.x - a.x)/(b.x - a.x);
		else
			alpha = (ray.origin.y + t * ray.direction.y - a.y)/(b.y - a.y);

		if(alpha <= 1.0f && alpha >= 0.0f)
			result.hit = true;
	}

	result.t = t;

	return result;
}

ostream &operator<<(ostream &stream, const TraceResult &ray);
