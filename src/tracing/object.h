#pragma once

#include "math/allmath.h"
#include "tracing/shape.h"
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
} // namespace ACTION_PRESETS

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
  dvec2 pos;
  dvec2 up;
  unsigned int subdivisions;
  shared_ptr<Tree> root;

  void buildTree(unsigned int subdivisions);

public:
  Object2D(const dvec2 &_pos = {0.0, 0.0},
           const dvec2 &_up = {0.0, 1.0}, unsigned int _subdivisions = 0);

  void setPos(const dvec2 &_pos) { pos = _pos; }
  void setUp(const dvec2 &_up) { up = _up; }

  const vector<shared_ptr<Shape2D>> &getShapes() const { return shapes; }

  vector<shared_ptr<Shape2D>> getAABBs() const {
    vector<shared_ptr<Shape2D>> boxes;
    root->forEach([&](shared_ptr<Tree> t) { boxes.push_back(t->box); });
    return boxes;
  }

  dvec2 getPos() const { return pos; }
  dvec2 getUp() const { return up; }

  IntersectResult2D intersect(const Ray2D &ray) const;

  void init() {
    shapes = build();
    buildTree(subdivisions);
  }

  virtual vector<shared_ptr<Shape2D>> build() = 0;

  virtual void action(Ray2D &ray, const IntersectResult2D &result,
                      vector<Ray2D> &createdRays) = 0;
};
