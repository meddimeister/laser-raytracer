#include "tracing/shape.h"
#include <limits>

AABB2D::AABB2D(const vector<dvec2> &points) {
  if (points.empty()) {
    bmin = {0.0, 0.0};
    bmax = {0.0, 0.0};
  } else {
    double xMin = std::numeric_limits<double>::max(), yMin = xMin;
    double xMax = -xMin, yMax = -xMin;
    for (const auto &p : points) {
      if (p.x < xMin)
        xMin = p.x;
      if (p.y < yMin)
        yMin = p.y;
      if (p.x > xMax)
        xMax = p.x;
      if (p.y > yMax)
        yMax = p.y;
    }
    bmin = {xMin, yMin};
    bmax = {xMax, yMax};
  }
}

dvec2 AABB2D::getMidPoint() const {
  return {(bmin.x + bmax.x) / 2, (bmin.y + bmax.y) / 2};
}

bool AABB2D::isInside(const dvec2 &point) const {
  if (bmin.x <= point.x && point.x < bmax.x) {
    if (bmin.y <= point.y && point.y < bmax.y) {
      return true;
    }
  }
  return false;
}

Line2D::Line2D(const dvec2 &_a, const dvec2 &_b) : a(_a), b(_b) {
  aabb = AABB2D({a, b});
}

IntersectResult2D Line2D::intersect(const Ray2D &ray) const {

  IntersectResult2D result;

  dvec2 bma = b - a;
  dvec2 ame = a - ray.origin;
  double dxbma = cross(ray.direction, bma);
  double alpha = cross(ame, ray.direction) / dxbma;

  if (alpha >= 0.0 && alpha <= 1.0) {
    double t = cross(ame, bma) / dxbma;
    if (t > 0.0) {
      result.tEnter = t;
      result.tLeave = t;
      result.hit = true;
      result.normalEnter = normalize(dvec2(a.y - b.y, b.x - a.x));
      result.normalLeave = result.normalEnter;
    }
  }
  return result;
}

vector<vec4> Line2D::lineRepresentation() const {
  return {{a.x, a.y, b.x, b.y}};
}

BoundingBox2D::BoundingBox2D(const dvec2 &_bmin, const dvec2 &_bmax) {
  aabb = AABB2D({_bmin, _bmax});
}

BoundingBox2D::BoundingBox2D(const vector<dvec2> &points) {
  aabb = AABB2D(points);
}

BoundingBox2D::BoundingBox2D(const AABB2D &_aabb) { aabb = _aabb; }

BoundingBox2D::BoundingBox2D(const vector<AABB2D> &aabbs) {
  vector<dvec2> points;
  for (const auto &_aabb : aabbs) {
    points.push_back(_aabb.bmin);
    points.push_back(_aabb.bmax);
  }
  aabb = AABB2D(points);
}

BoundingBox2D::BoundingBox2D(const vector<BoundingBox2D> &aabbs) {
  vector<dvec2> points;
  for (const auto &_aabb : aabbs) {
    points.push_back(_aabb.aabb.bmin);
    points.push_back(_aabb.aabb.bmax);
  }
  aabb = AABB2D(points);
}

IntersectResult2D BoundingBox2D::intersect(const Ray2D &ray) const {
  IntersectResult2D ret;

  double tx1 = (aabb.bmin.x - ray.origin.x) / ray.direction.x;
  double tx2 = (aabb.bmax.x - ray.origin.x) / ray.direction.x;

  double tmin = glm::min(tx1, tx2);
  double tmax = glm::max(tx1, tx2);

  double ty1 = (aabb.bmin.y - ray.origin.y) / ray.direction.y;
  double ty2 = (aabb.bmax.y - ray.origin.y) / ray.direction.y;

  tmin = glm::max(tmin, glm::min(ty1, ty2));
  tmax = glm::min(tmax, glm::max(ty1, ty2));

  if (tmax >= tmin) {
    if (tmin >= 0.0) {
      ret.hit = true;
      ret.tEnter = tmin;
      ret.tLeave = tmax;
    } else if (aabb.isInside(ray.origin)) {
      ret.hit = true;
      ret.tEnter = 0.0;
      ret.tLeave = tmax;
    }

    if (tmin == tx1) {
      ret.normalEnter = {-1.0, 0.0};
    } else if (tmin == tx2) {
      ret.normalEnter = {1.0, 0.0};
    } else if (tmin == ty1) {
      ret.normalEnter = {0.0, -1.0};
    } else if (tmin == ty2) {
      ret.normalEnter = {0.0, 1.0};
    }
    if (tmax == tx1) {
      ret.normalLeave = {-1.0, 0.0};
    } else if (tmax == tx2) {
      ret.normalLeave = {1.0, 0.0};
    } else if (tmax == ty1) {
      ret.normalLeave = {0.0, -1.0};
    } else if (tmax == ty2) {
      ret.normalLeave = {0.0, 1.0};
    }
  }

  return ret;
}

IntersectResult2D BoundingBox2D::intersectCheck(const Ray2D &ray) const {
  IntersectResult2D ret;

  double tx1 = (aabb.bmin.x - ray.origin.x) / ray.direction.x;
  double tx2 = (aabb.bmax.x - ray.origin.x) / ray.direction.x;

  double tmin = glm::min(tx1, tx2);
  double tmax = glm::max(tx1, tx2);

  double ty1 = (aabb.bmin.y - ray.origin.y) / ray.direction.y;
  double ty2 = (aabb.bmax.y - ray.origin.y) / ray.direction.y;

  tmin = glm::max(tmin, glm::min(ty1, ty2));
  tmax = glm::min(tmax, glm::max(ty1, ty2));

  if (tmax >= tmin) {
    if (tmin >= 0.0) {
      ret.hit = true;
      ret.tEnter = tmin;
      ret.tLeave = tmax;
    } else if (aabb.isInside(ray.origin)) {
      ret.hit = true;
      ret.tEnter = 0.0;
      ret.tLeave = tmax;
    }
  }

  return ret;
}

Sphere2D::Sphere2D(const dvec2 &_center, double _radius, const dvec2 &_facing,
                   double _maxAngle)
    : center(_center), radius(_radius), facing(normalize(_facing)),
      maxAngle(_maxAngle) {
  vector<dvec2> points;

  if (abs(angle({-radius, 0.0}, facing) < maxAngle / 2.0))
    points.push_back({center.x - radius, center.y});
  if (abs(angle({radius, 0.0}, facing) < maxAngle / 2.0))
    points.push_back({center.x + radius, center.y});
  if (abs(angle({0.0, -radius}, facing) < maxAngle / 2.0))
    points.push_back({center.x, center.y - radius});
  if (abs(angle({0.0, radius}, facing) < maxAngle / 2.0))
    points.push_back({center.x, center.y + radius});

  points.push_back(rotate(radius * facing, -maxAngle / 2.0) + center);
  points.push_back(rotate(radius * facing, maxAngle / 2.0) + center);

  aabb = AABB2D(points);
}

IntersectResult2D Sphere2D::intersect(const Ray2D &ray) const {
  IntersectResult2D ret;

  double a = dot(ray.direction, ray.direction);
  double b = 2.0 * ray.direction.x * (ray.origin.x - center.x) +
             2.0 * ray.direction.y * (ray.origin.y - center.y);
  double c = dot(ray.origin - center, ray.origin - center) - radius * radius;

  double discriminante = sqrt(b * b - 4.0 * a * c);
  if (discriminante > 0.0) {
    double tEnter = (-b - discriminante) / (2.0 * a);
    double tLeave = (-b + discriminante) / (2.0 * a);

    if (tEnter >= 0.0) {
      dvec2 enter = ray.origin + tEnter * ray.direction;
      dvec2 leave = ray.origin + tLeave * ray.direction;
      dvec2 normalEnter = normalize(enter - center);
      dvec2 normalLeave = normalize(leave - center);
      if (abs(angle(normalEnter, facing)) <= maxAngle / 2.0) {
        ret.tEnter = tEnter;
        ret.normalEnter = normalize(enter - center);
        ret.hit = true;
        if (abs(angle(normalLeave, facing)) <= maxAngle / 2.0) {
          ret.tLeave = tLeave;
          ret.normalLeave = normalize(leave - center);
          return ret;
        } else {
          ret.tLeave = tEnter;
          ret.normalLeave = -normalize(enter - center);
          return ret;
        }
      }
      if (abs(angle(normalLeave, facing)) <= maxAngle / 2.0) {
        ret.tEnter = tLeave;
        ret.tLeave = tLeave;
        ret.normalEnter = -normalize(leave - center);
        ret.normalLeave = normalize(leave - center);
        ret.hit = true;
        return ret;
      }
    }
  }
  return ret;
}

vector<vec4> Sphere2D::lineRepresentation() const {
  vector<vec4> lines;
  double segmentAngle = maxAngle / 100;
  dvec2 a = rotate(radius * facing, -maxAngle / 2.0);
  for (int i = 0; i < 100; ++i) {
    dvec2 b = rotate(a, segmentAngle);
    lines.push_back(
        {a.x + center.x, a.y + center.y, b.x + center.x, b.y + center.y});
    a = b;
  }
  return lines;
}

vector<vec4> BoundingBox2D::lineRepresentation() const {
  return {{aabb.bmin.x, aabb.bmin.y, aabb.bmin.x, aabb.bmax.y},
          {aabb.bmax.x, aabb.bmin.y, aabb.bmax.x, aabb.bmax.y},
          {aabb.bmin.x, aabb.bmin.y, aabb.bmax.x, aabb.bmin.y},
          {aabb.bmin.x, aabb.bmax.y, aabb.bmax.x, aabb.bmax.y}};
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

ostream &operator<<(ostream &stream, const Line2D &line) {
  return stream << "Line2D: {a: [" << line.a.x << ", " << line.a.y << "]"
                << ", b: [" << line.b.x << ", " << line.b.y << "]"
                << "}";
}
