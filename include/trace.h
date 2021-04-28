#pragma once

#include "glm.hpp"
#include "ray.h"
#include <iostream>

using namespace std;
using namespace glm;

struct TraceResult2D {
	float t;
	bool hit = false;
	vec2 normal;
};

struct TraceResult3D {
	float t;
	bool hit = false;
	vec3 normal;
};

inline TraceResult2D intersectLine(const vec2 &a , const vec2 &b, const Ray2D &ray){

	TraceResult2D result;

	float t = ((ray.origin.x - a.x)*(b.y - a.y) + (ray.origin.y - a.y)*(b.x - a.x))/
		(ray.direction.y*(b.x - a.x) - ray.direction.x*(b.y - a.y));
	
	if(t >= 0.0f){
		float alpha;

		if(b.x != a.x)
			alpha = (ray.origin.x + t * ray.direction.x - a.x)/(b.x - a.x);
		else
			alpha = (ray.origin.y + t * ray.direction.y - a.y)/(b.y - a.y);

		if(alpha <= 1.0f && alpha >= 0.0f){
			result.hit = true;
			result.normal = normalize(vec2(a.y - b.y, b.x - a.x));
		}
	}

	result.t = t;

	return result;
}

inline Ray2D reflect(const Ray2D &incoming, float t, const vec2 &normal){
	vec2 origin = incoming.origin + t * incoming.direction;
	vec2 direction = reflect(incoming.direction, normal);
	return Ray2D(origin, direction);
}

inline Ray3D reflect(const Ray3D &incoming, float t, const vec3 &normal){
	vec3 origin = incoming.origin + t * incoming.direction;
	vec3 direction = reflect(incoming.direction, normal);
	return Ray3D(origin, direction);
}

ostream &operator<<(ostream &stream, const TraceResult2D &ray);
ostream &operator<<(ostream &stream, const TraceResult2D &ray);
