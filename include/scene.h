#pragma once

#include "object.h"

class Scene2D {
public:
	vector<shared_ptr<Object2D>> objects;

	void add(const shared_ptr<Object2D> &object);

	vector<vector<Ray2D>> trace(vector<Ray2D> &rays, unsigned int depth = 1);
};
