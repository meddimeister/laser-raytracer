#include "object.h"
#include <algorithm>

void Object2D::setPos(const vec2 &_pos) { pos = _pos; }
void Object2D::setUp(const vec2 &_up) { up = _up; }
void Object2D::setScale(const vec2 &_scale) { scale = _scale; }

const vector<shared_ptr<Shape2D>> &Object2D::getShapes() const {
  return shapes;
}
vec2 Object2D::getPos() const { return pos; }
vec2 Object2D::getUp() const { return up; }
vec2 Object2D::getScale() const { return scale; }

IntersectResult2D Object2D::intersect(Ray2D &ray) {
  IntersectResult2D ret;
  ret.tEnter = MAXFLOAT;
  ret.tLeave = -MAXFLOAT;
  ret.hit = false;

  for (auto &shape : shapes) {
    IntersectResult2D result = shape->intersect(ray);
    if (result.hit) {
			ret.hit = true;
			if(result.tEnter < ret.tEnter){
      	ret.tEnter = result.tEnter;
      	ret.normalEnter = result.normalEnter;
			}
			if(result.tLeave > ret.tLeave){
				ret.tLeave = result.tLeave;
				ret.normalLeave = result.normalLeave;
			}
    }
  }

  return ret;
}

vector<IntersectResult2D> Object2D::intersect(vector<Ray2D> &rays) {
  vector<IntersectResult2D> results;
  for (auto &ray : rays) {
    results.push_back(intersect(ray));
  }
  return results;
}

vector<Ray2D> Object2D::reflect(vector<Ray2D> &rays) {
  vector<Ray2D> reflections;
  for (auto &ray : rays) {
    IntersectResult2D result = intersect(ray);

    if (result.hit)
      reflections.push_back(ray.reflect(result.tEnter, result.normalEnter));
  }
  return reflections;
}

vector<IntersectResult2D> Object2D::pass(Ray2D &ray) {
  vector<IntersectResult2D> results;
  for (auto &shape : shapes) {
    IntersectResult2D result = shape->intersect(ray);
    if (result.hit)
      results.push_back(result);
  }
  return results;
}
