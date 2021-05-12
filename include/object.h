#pragma once

#include "math.h"
#include "shape.h"
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

enum OBJECT_TYPE { PASS, ABSORB, REFLECT };

class Object2D {
  struct Tree {
    shared_ptr<BoundingBox2D> box;
    vector<shared_ptr<Shape2D>> shapes;
    vector<shared_ptr<Tree>> children;

    void subdivide(unsigned int maxdepth = 1, unsigned int currdepth = 0);
    void forEach(function<void(shared_ptr<Tree>)> func) const;
    void forEachConditional(function<bool(shared_ptr<Tree>)> func) const;
  };

  void buildTree(unsigned int subdivisions);

protected:
  OBJECT_TYPE type;
  vector<shared_ptr<Shape2D>> shapes;
  shared_ptr<Tree> root;
  vec2 pos;
  vec2 up;
  vec2 scale;

public:
  Object2D(OBJECT_TYPE _type, const vector<shared_ptr<Shape2D>> &&_shapes,
           unsigned int _subdivisions = 0, const vec2 &_pos = {0.0f, 0.0f},
           const vec2 &_up = {0.0f, 1.0f}, const vec2 &_scale = {1.0f, 1.0f});

  void setPos(const vec2 &_pos);
  void setUp(const vec2 &_up);
  void setScale(const vec2 &_scale);

  const vector<shared_ptr<Shape2D>> &getShapes() const;
  vector<shared_ptr<Shape2D>> getAABBs() const;
  vec2 getPos() const;
  vec2 getUp() const;
  vec2 getScale() const;
  OBJECT_TYPE getType() const;

  IntersectResult2D intersect(Ray2D &ray) const;
  vector<IntersectResult2D> intersect(vector<Ray2D> &ray) const;
  vector<Ray2D> reflect(vector<Ray2D> &ray) const;
  vector<IntersectResult2D> pass(Ray2D &ray) const;
};
