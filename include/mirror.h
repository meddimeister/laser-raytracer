#pragma once

#include "allmath.h"
#include "object.h"
#include "shape.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace glm;

vector<shared_ptr<Shape2D>> build(const dvec2 &_pos, const dvec2 &_opticalAxis,
                                  const function<dvec2(double)> _shapeFunction,
                                  int _segments);

class Mirror2D : public Object2D {
private:
  function<dvec2(double)> shapeFunction;
  int segments;

public:
  Mirror2D(const dvec2 &_pos, const dvec2 &_opticalAxis,
           function<dvec2(double)> _shapeFunction, int _segments)
      : Object2D(build(_pos, _opticalAxis, _shapeFunction, _segments), 4, _pos,
                 _opticalAxis),
        shapeFunction(_shapeFunction), segments(_segments){};

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays) {
    ACTION_PRESETS::reflect(ray, result, createdRays);
  }

  void rebuild() {
    shapes = build(pos, up, shapeFunction, segments);
    buildTree(subdivisions);
  }
};
