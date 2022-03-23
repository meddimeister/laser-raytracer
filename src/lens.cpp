#include "lens.h"
#include <cmath>

vector<shared_ptr<Shape2D>> build(const dvec2 &_pos, const dvec2 &_opticalAxis,
                                  double _radius) {
  vector<shared_ptr<Shape2D>> shapes;
  dvec2 a = _pos + _radius * normalize(rotate(_opticalAxis, 0.5 * double(M_PI)));
  dvec2 b =
      _pos + _radius * normalize(rotate(_opticalAxis, -0.5 * double(M_PI)));
  shapes.push_back(make_shared<Line2D>(Line2D(a, b)));
  return shapes;
}
