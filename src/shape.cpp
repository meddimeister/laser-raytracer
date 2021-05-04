#include "shape.h"

vector<vec4> Line2D::lineRepresentation() const {
  return {{a.x, a.y, b.x, b.y}};
}

vector<vec4> AABB2D::lineRepresentation() const {
  return {{bmin.x, bmin.y, bmin.x, bmax.y},
          {bmax.x, bmin.y, bmax.x, bmax.y},
          {bmin.x, bmin.y, bmax.x, bmin.y},
          {bmin.x, bmax.y, bmax.x, bmax.y}};
}

ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult) {
  return stream << "IntersectResult2D: {tEnter: " << intersectResult.tEnter
                << ", tLeave: " << intersectResult.tLeave
                << ", hit: " << intersectResult.hit << ", normalEnter: ["
                << intersectResult.normalEnter.x << ", "
                << intersectResult.normalEnter.y << "]"
                << ", normalLeave: [" << intersectResult.normalLeave.x << ", "
                << intersectResult.normalLeave.y << "]"
                << "}";
}

ostream &operator<<(ostream &stream, const IntersectResult3D &intersectResult) {
  return stream << "IntersectResult3D: {tEnter: " << intersectResult.tEnter
                << ", tLeave: " << intersectResult.tLeave
                << ", hit: " << intersectResult.hit << ", normalEnter: ["
                << intersectResult.normalEnter.x << ", "
                << intersectResult.normalEnter.y << ", "
                << intersectResult.normalEnter.z << "]"
                << ", normalLeave: [" << intersectResult.normalLeave.x << ", "
                << intersectResult.normalLeave.y << ", "
                << intersectResult.normalLeave.z << "]"
                << "}";
}

ostream &operator<<(ostream &stream, const Line2D &line) {
  return stream << "Line2D: {a: [" << line.a.x << ", " << line.a.y << "]"
                << ", b: [" << line.b.x << ", " << line.b.y << "]"
                << "}";
}
