#pragma once

#include "object.h"

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin,
                                  const vec2 &_bmax);

class Crystal2D : public Object2D {
public:
  Crystal2D(const vec2 &_pos, const vec2 &_bmin, const vec2 &_bmax)
      : Object2D(ABSORB, build(_pos, _bmin, _bmax), 0, _pos) {}
};
