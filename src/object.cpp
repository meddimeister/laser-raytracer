#include "object.h"
#include "shape.h"
#include <algorithm>
#include <queue>
#include <limits>

namespace ACTION_PRESETS
{
  void pass(Ray2D &ray, const IntersectResult2D &result,
            vector<Ray2D> &createdRays) {}

  void absorb(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays)
  {
    ray.terminate(result.tEnter);
  }

  void reflect(Ray2D &ray, const IntersectResult2D &result,
               vector<Ray2D> &createdRays)
  {
    ray.terminate(result.tEnter);
    createdRays.push_back(ray.reflect(result.tEnter, result.normalEnter));
  }
} // namespace ACTION_PRESETS

void Object2D::Tree::forEach(function<void(shared_ptr<Tree>)> func) const
{
  queue<shared_ptr<Tree>> q;
  q.push(make_shared<Tree>(*this));

  while (!q.empty())
  {
    auto t = q.front();
    func(t);
    for (auto c : t->children)
      q.push(c);
    q.pop();
  }
}

void Object2D::Tree::forEachConditional(
    function<bool(shared_ptr<Tree>)> func) const
{
  queue<shared_ptr<Tree>> q;
  q.push(make_shared<Tree>(*this));

  while (!q.empty())
  {
    auto t = q.front();
    if (func(t))
    {
      for (auto c : t->children)
        q.push(c);
    }
    q.pop();
  }
}

void Object2D::Tree::subdivide(unsigned int maxdepth, unsigned int currdepth)
{
  if (currdepth < maxdepth)
  {
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

    for (auto it = shapes.begin(); it != shapes.end();)
    {
      auto shape = *it;
      vec2 midpoint = shape->aabb.getMidPoint();
      bool erased = false;
      for (unsigned int i = 0; i < 4; ++i)
      {
        if (subs[i].isInside(midpoint))
        {
          subshapes[i].push_back(shape);
          it = shapes.erase(it);
          erased = true;
          break;
        }
      }
      if (!erased)
        it++;
    }

    for (unsigned int i = 0; i < 4; ++i)
    {
      if (subshapes[i].empty())
        continue;
      Tree child;
      vector<AABB2D> aabbs;
      for (const auto &shape : subshapes[i])
      {
        aabbs.push_back(shape->aabb);
      }
      child.box = make_shared<BoundingBox2D>(aabbs);
      child.shapes = subshapes[i];
      child.subdivide(maxdepth, currdepth + 1);
      children.push_back(make_shared<Tree>(child));
    }
  }
}

void Object2D::buildTree(unsigned int subdivisions)
{
  vector<AABB2D> aabbs;
  for (const auto &shape : shapes)
  {
    aabbs.push_back(shape->aabb);
  }
  root = make_shared<Tree>();
  root->box = make_shared<BoundingBox2D>(aabbs);
  root->shapes = shapes;
  root->subdivide(subdivisions);
}

Object2D::Object2D(const vector<shared_ptr<Shape2D>> &&_shapes,
                   unsigned int _subdivisions, const vec2 &_pos,
                   const vec2 &_up, const vec2 &_scale)
    : shapes(_shapes), subdivisions(_subdivisions), pos(_pos), up(_up), scale(_scale)
{
  buildTree(_subdivisions);
}

IntersectResult2D Object2D::intersect(const Ray2D &ray) const
{
  IntersectResult2D ret;
  ret.tEnter = std::numeric_limits<float>::max();
  ret.tLeave = -std::numeric_limits<float>::max();
  ret.hit = false;

  root->forEachConditional([&](shared_ptr<Tree> t)
                           {
                             IntersectResult2D boxResult = t->box->intersectCheck(ray);
                             if (boxResult.hit)
                             {
                               // intersect shapes if any there
                               for (auto &shape : t->shapes)
                               {
                                 IntersectResult2D shapeResult = shape->intersect(ray);
                                 if (shapeResult.hit)
                                 {
                                   ret.hit = true;
                                   if (shapeResult.tEnter < ret.tEnter)
                                   {
                                     ret.tEnter = shapeResult.tEnter;
                                     ret.normalEnter = shapeResult.normalEnter;
                                   }
                                   if (shapeResult.tLeave > ret.tLeave)
                                   {
                                     ret.tLeave = shapeResult.tLeave;
                                     ret.normalLeave = shapeResult.normalLeave;
                                   }
                                 }
                               }
                               return true;
                             }
                             return false;
                           });

  return ret;
}
