#pragma once

#include "object.h"

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin,
                                  const vec2 &_bmax);

class Grid2D : public Object2D {
public:
	unsigned int hits;

  Grid2D(const vec2 &_pos, const vec2 &_bmin, const vec2 &_bmax)
      : Object2D(build(_pos, _bmin, _bmax), 0, _pos) {}

  void action(Ray2D &ray, const IntersectResult2D &result,
                      vector<Ray2D> &createdRays) {
		hits++;
		ACTION_PRESETS::pass(ray, result, createdRays);
  }
};
