#include "lens.h"

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_opticalAxis,
                                  float _thickness, const vec2 &_radii){
	vector<shared_ptr<Shape2D>> shapes;
	shapes.push_back(make_shared<Sphere2D>(_pos, _thickness));
  return shapes;
}

