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
  function<dvec2(double)> _shapeFunction;
  int _segments;

public:
  Mirror2D(const dvec2 &pos, const dvec2 &up,
           function<dvec2(double)> shapeFunction, int segments)
      : Object2D({}, 4, pos, up),
        _shapeFunction(shapeFunction), _segments(segments){
          init();
        };

  vector<shared_ptr<Shape2D>> build() {
    vector<shared_ptr<Shape2D>> lines;
    for (int i = 0; i < _segments; ++i) {
      dvec2 ap = _shapeFunction(i * (1.0 / _segments));
      dvec2 bp = _shapeFunction((i + 1) * (1.0 / _segments));

      dvec2 am = {-ap.x, ap.y};
      dvec2 bm = {-bp.x, bp.y};

      double rotationAngle = orientedAngle({0.0, 1.0}, up);

      ap = rotate(ap, rotationAngle);
      bp = rotate(bp, rotationAngle);

      am = rotate(am, rotationAngle);
      bm = rotate(bm, rotationAngle);

      ap += pos;
      bp += pos;

      am += pos;
      bm += pos;

      lines.push_back(make_shared<Line2D>(ap, bp));
      lines.push_back(make_shared<Line2D>(am, bm));
    }
    return lines;
  }

  void action(Ray2D &ray, const IntersectResult2D &result,
              vector<Ray2D> &createdRays) {
    ACTION_PRESETS::reflect(ray, result, createdRays);
  }

  void rebuild() {
    // shapes = build(pos, up, shapeFunction, segments);
    // buildTree(subdivisions);
    init();
  }
};
