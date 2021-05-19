#pragma once

#include "object.h"
#include <iostream>

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin,
                                  const vec2 &_bmax);

class Crystal2D : public Object2D {
public:
  Crystal2D(const vec2 &_pos, const vec2 &_bmin, const vec2 &_bmax)
      : Object2D(build(_pos, _bmin, _bmax), ACTION_PRESETS::pass, 0, _pos) {}
};
