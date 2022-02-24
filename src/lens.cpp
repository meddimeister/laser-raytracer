#include "lens.h"
#include <cmath>

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_opticalAxis,
                                  float _radius) {
  vector<shared_ptr<Shape2D>> shapes;
  vec2 a = _pos + _radius * normalize(rotate(_opticalAxis, 0.5f * float(M_PI)));
  vec2 b =
      _pos + _radius * normalize(rotate(_opticalAxis, -0.5f * float(M_PI)));
  shapes.push_back(make_shared<Line2D>(Line2D(a, b)));
  return shapes;
}
