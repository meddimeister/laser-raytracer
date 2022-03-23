#include "mirror.h"

vector<shared_ptr<Shape2D>> build(const dvec2 &_pos, const dvec2 &_opticalAxis,
                                  const function<dvec2(double)> _shapeFunction,
                                  int _segments) {
  vector<shared_ptr<Shape2D>> lines;
  for (int i = 0; i < _segments; ++i) {
    dvec2 ap = _shapeFunction(i * (1.0 / _segments));
    dvec2 bp = _shapeFunction((i + 1) * (1.0 / _segments));

    dvec2 am = {-ap.x, ap.y};
    dvec2 bm = {-bp.x, bp.y};

    double rotationAngle = orientedAngle({0.0, 1.0}, _opticalAxis);

    ap = rotate(ap, rotationAngle);
    bp = rotate(bp, rotationAngle);

    am = rotate(am, rotationAngle);
    bm = rotate(bm, rotationAngle);

    ap += _pos;
    bp += _pos;

    am += _pos;
    bm += _pos;

    lines.push_back(make_shared<Line2D>(ap, bp));
    lines.push_back(make_shared<Line2D>(am, bm));
  }
  return lines;
}
