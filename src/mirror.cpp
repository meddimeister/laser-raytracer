#include "mirror.h"

vector<shared_ptr<Shape2D>> Mirror2D::build() {
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

void Mirror2D::action(Ray2D &ray, const IntersectResult2D &result,
                      vector<Ray2D> &createdRays) {
  ACTION_PRESETS::reflect(ray, result, createdRays);
}