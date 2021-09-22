#include "mirror.h"

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_opticalAxis,
                                  const function<float(float)> _shapeFunction, int _segments)
{
  vector<shared_ptr<Shape2D>> lines;
  for (int i = 0; i < _segments; ++i)
  {
    vec2 ap = {i * (1.0f / _segments),
               _shapeFunction(i * (1.0f / _segments))};
    vec2 bp = {(i + 1) * (1.0f / _segments),
               _shapeFunction((i + 1) * (1.0f / _segments))};

    vec2 am = {-i * (1.0f / _segments),
               _shapeFunction(-i * (1.0f / _segments))};
    vec2 bm = {-(i + 1) * (1.0f / _segments),
               _shapeFunction(-(i + 1) * (1.0f / _segments))};

    float rotationAngle = orientedAngle({0.0f, 1.0f}, _opticalAxis);

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
