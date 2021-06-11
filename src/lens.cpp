#include "lens.h"

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_opticalAxis,
                                  float _width, float _height, const vec2 &_radii){
	vector<shared_ptr<Shape2D>> shapes;
	shapes.push_back(make_shared<Sphere2D>(Sphere2D(_pos, _radii.x, {1.0f, 0.0f}, M_PI/4.0f)));
	shapes.push_back(make_shared<Sphere2D>(Sphere2D(_pos, _radii.y, {-1.0f, 0.0f}, M_PI/4.0f)));
  return shapes;
}

