#pragma once

#include "ray.h"
#include "math.h"
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

	vec2 bma = b - a;
	vec2 ame = a - ray.origin;
	float dxbma = cross(ray.direction, bma);
	float alpha = cross(ame, ray.direction)/dxbma;

	if(alpha >= 0.0f && alpha <= 1.0f){
		float t = cross(ame, bma)/dxbma;
		if(t >= 0.0f){
			result.t = t;
			result.hit = true;
			result.normal = normalize(vec2(a.y - b.y, b.x - a.x));
		}
	}

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
