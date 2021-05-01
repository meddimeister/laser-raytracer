#include "shape.h"

vector<vec4> Line2D::lineRepresentation() { return {{a.x, a.y, b.x, b.y}}; }

ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult) {
  return stream << "IntersectResult2D: {t: " << intersectResult.t
                << ", hit: " << intersectResult.hit << ", normal: ["
                << intersectResult.normal.x << ", " << intersectResult.normal.y
                << "]"
                << "}";
}

ostream &operator<<(ostream &stream, const IntersectResult3D &intersectResult) {
  return stream << "IntersectResult3D: {t: " << intersectResult.t
                << ", hit: " << intersectResult.hit << ", normal: ["
                << intersectResult.normal.x << ", " << intersectResult.normal.y
                << ", " << intersectResult.normal.z << "]"
                << "}";
}

ostream &operator<<(ostream &stream, const Line2D &line) {
  return stream << "Line2D: {a: [" << line.a.x << ", " << line.a.y << "]"
                << ", b: [" << line.b.x << ", " << line.b.y << "]"
                << "}";
}
