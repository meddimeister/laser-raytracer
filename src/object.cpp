#include "object.h"

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
  ret.t = MAXFLOAT;
  ret.hit = false;

  for (auto &shape : shapes) {
    IntersectResult2D result = shape->intersect(ray);
    if (result.hit && result.t < ret.t) {
      ret.t = result.t;
      ret.hit = true;
      ret.normal = result.normal;
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
      reflections.push_back(ray.reflect(result.t, result.normal));
  }
  return reflections;
}
