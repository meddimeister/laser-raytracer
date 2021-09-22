#pragma once

#include "allmath.h"
#include "shape.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

namespace ACTION_PRESETS {
  void pass(Ray2D &ray, const IntersectResult2D &result,
            vector<Ray2D> &createdRays);

  void absorb(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays);

  void reflect(Ray2D &ray, const IntersectResult2D &result,
               vector<Ray2D> &createdRays);
}// namespace ACTION_PRESETS

class Object2D {
  struct Tree {
    shared_ptr<BoundingBox2D> box;
    vector<shared_ptr<Shape2D>> shapes;
    vector<shared_ptr<Tree>> children;

    void subdivide(unsigned int maxdepth = 1, unsigned int currdepth = 0);
    void forEach(function<void(shared_ptr<Tree>)> func) const;
    void forEachConditional(function<bool(shared_ptr<Tree>)> func) const;
  };

protected:
  vector<shared_ptr<Shape2D>> shapes;
  unsigned int subdivisions;
  vec2 pos;
  vec2 up;
  vec2 scale;
  shared_ptr<Tree> root;
  

  void buildTree(unsigned int subdivisions);

public:
  Object2D(const vector<shared_ptr<Shape2D>> &&_shapes,
           unsigned int _subdivisions = 0, const vec2 &_pos = {0.0f, 0.0f},
           const vec2 &_up = {0.0f, 1.0f}, const vec2 &_scale = {1.0f, 1.0f});

  void setPos(const vec2 &_pos) { pos = _pos; }
  void setUp(const vec2 &_up) { up = _up; }
  void setScale(const vec2 &_scale) { scale = _scale; }

  const vector<shared_ptr<Shape2D>> &getShapes() const { return shapes; }

  vector<shared_ptr<Shape2D>> getAABBs() const {
    vector<shared_ptr<Shape2D>> boxes;
    root->forEach([&](shared_ptr<Tree> t) { boxes.push_back(t->box); });
    return boxes;
  }

  vec2 getPos() const { return pos; }
  vec2 getUp() const { return up; }
  vec2 getScale() const { return scale; }

  IntersectResult2D intersect(const Ray2D &ray) const;

  virtual void action(Ray2D &ray, const IntersectResult2D &result,
                      vector<Ray2D> &createdRays) {
		ACTION_PRESETS::pass(ray, result, createdRays);
  }
};
