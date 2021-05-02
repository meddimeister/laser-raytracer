#pragma once

#include "object.h"

class Crystal2D : public Object2D {
public:
	Crystal2D(const vec2 &_pos, const vec2 &_bmin, const vec2 &_bmax) : Object2D(_pos){
		shapes.push_back(make_shared<AABB2D>(_pos + _bmin, _pos + _bmax));
	}
};
