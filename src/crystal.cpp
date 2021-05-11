#include "crystal.h"

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin, const vec2 &_bmax) {
  vector<shared_ptr<Shape2D>> boxes;
  boxes.push_back(make_shared<BoundingBox2D>(_pos + _bmin, _pos + _bmax));
  return boxes;
}
