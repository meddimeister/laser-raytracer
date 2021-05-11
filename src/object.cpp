#include "object.h"
#include "shape.h"
#include <algorithm>
#include <queue>

void Object2D::Tree::forEach(
    const function<void(shared_ptr<Tree>)> func) const {
  queue<shared_ptr<Tree>> q;
  q.push(make_shared<Tree>(*this));

  while (!q.empty()) {
    auto t = q.front();
    func(t);
    for (auto c : t->children)
      q.push(c);
    q.pop();
  }
}

void Object2D::Tree::subdivide(unsigned int maxdepth, unsigned int currdepth) {
  if (currdepth < maxdepth) {
    AABB2D subs[4];
    vec2 bmin = box->aabb.bmin;
    vec2 bmax = box->aabb.bmax;

    subs[0] = AABB2D(
        {{bmin.x, bmin.y}, {(bmin.x + bmax.x) / 2, (bmin.y + bmax.y) / 2}});
    subs[1] = AABB2D(
        {{(bmin.x + bmax.x) / 2, bmin.y}, {bmax.x, (bmin.y + bmax.y) / 2}});
    subs[2] = AABB2D(
        {{bmin.x, (bmin.y + bmax.y) / 2}, {(bmin.x + bmax.x) / 2, bmax.y}});
    subs[3] = AABB2D(
        {{(bmin.x + bmax.x) / 2, (bmin.y + bmax.y) / 2}, {bmax.x, bmax.y}});

    vector<shared_ptr<Shape2D>> subshapes[4];

    for (const auto &shape : shapes) {
      vec2 midpoint = shape->aabb.getMidPoint();
      for (unsigned int i = 0; i < 4; ++i) {
        if (subs[i].isInside(midpoint)) {
          subshapes[i].push_back(shape);
          break;
        }
      }
    }

    // shapes.clear();

    for (unsigned int i = 0; i < 4; ++i) {
      if (subshapes[i].empty())
        continue;
      Tree child;
      vector<AABB2D> aabbs;
      for (const auto &shape : subshapes[i]) {
        aabbs.push_back(shape->aabb);
      }
      child.box = make_shared<BoundingBox2D>(aabbs);
      child.shapes = subshapes[i];
      child.subdivide(maxdepth, currdepth + 1);
      children.push_back(make_shared<Tree>(child));
    }
  }
}

void Object2D::buildTree() {
  vector<AABB2D> aabbs;
  for (const auto &shape : shapes) {
    aabbs.push_back(shape->aabb);
  }
  root = make_shared<Tree>();
  root->box = make_shared<BoundingBox2D>(aabbs);
  root->shapes = shapes;
  root->subdivide(4);

  return;
}

Object2D::Object2D(OBJECT_TYPE _type,
                   const vector<shared_ptr<Shape2D>> &&_shapes,
                   const vec2 &_pos, const vec2 &_up, const vec2 &_scale)
    : type(_type), shapes(_shapes), pos(_pos), up(_up), scale(_scale) {
  buildTree();
}

void Object2D::setPos(const vec2 &_pos) { pos = _pos; }
void Object2D::setUp(const vec2 &_up) { up = _up; }
void Object2D::setScale(const vec2 &_scale) { scale = _scale; }

const vector<shared_ptr<Shape2D>> &Object2D::getShapes() const {
  return shapes;
}

vector<shared_ptr<Shape2D>> Object2D::getAABBs() const {
  vector<shared_ptr<Shape2D>> boxes;
  root->forEach([&](shared_ptr<Tree> t) { boxes.push_back(t->box); });
  return boxes;
}

vec2 Object2D::getPos() const { return pos; }
vec2 Object2D::getUp() const { return up; }
vec2 Object2D::getScale() const { return scale; }
OBJECT_TYPE Object2D::getType() const { return type; }

IntersectResult2D Object2D::intersect(Ray2D &ray) const {
  IntersectResult2D ret;
  ret.tEnter = MAXFLOAT;
  ret.tLeave = -MAXFLOAT;
  ret.hit = false;

  for (auto &shape : shapes) {
    IntersectResult2D result = shape->intersect(ray);
    if (result.hit) {
      ret.hit = true;
      if (result.tEnter < ret.tEnter) {
        ret.tEnter = result.tEnter;
        ret.normalEnter = result.normalEnter;
      }
      if (result.tLeave > ret.tLeave) {
        ret.tLeave = result.tLeave;
        ret.normalLeave = result.normalLeave;
      }
    }
  }

  return ret;
}

vector<IntersectResult2D> Object2D::intersect(vector<Ray2D> &rays) const {
  vector<IntersectResult2D> results;
  for (auto &ray : rays) {
    results.push_back(intersect(ray));
  }
  return results;
}

vector<Ray2D> Object2D::reflect(vector<Ray2D> &rays) const {
  vector<Ray2D> reflections;
  for (auto &ray : rays) {
    IntersectResult2D result = intersect(ray);

    if (result.hit) {
      ray.hit = true;
      ray.t = result.tEnter;
      reflections.push_back(ray.reflect(result.tEnter, result.normalEnter));
    }
  }
  return reflections;
}

vector<IntersectResult2D> Object2D::pass(Ray2D &ray) const {
  vector<IntersectResult2D> results;
  for (auto &shape : shapes) {
    IntersectResult2D result = shape->intersect(ray);
    if (result.hit)
      results.push_back(result);
  }
  return results;
}
