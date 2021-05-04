#pragma once

#include "object.h"

class Scene2D {
public:
  vector<shared_ptr<Object2D>> objects;
	vector<Ray2D> startrays;

  void add(const shared_ptr<Object2D> &object);

  void generatePointRays(const vec2 &origin, const vec2 &direction, float maxAngle,
                                  unsigned int count,
                                  const function<float()> &&lineDistribution);

  vector<vector<Ray2D>> trace(unsigned int depth = 1);
};
