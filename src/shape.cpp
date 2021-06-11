#include "shape.h"

AABB2D::AABB2D(const vector<vec2> &points) {
  if (points.empty()) {
    bmin = {0.0f, 0.0f};
    bmax = {0.0f, 0.0f};
  } else {
    float xMin = MAXFLOAT, yMin = MAXFLOAT;
    float xMax = -MAXFLOAT, yMax = -MAXFLOAT;
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

vec2 AABB2D::getMidPoint() const {
  return {(bmin.x + bmax.x) / 2, (bmin.y + bmax.y) / 2};
}

bool AABB2D::isInside(const vec2 &point) const {
  if (bmin.x <= point.x && point.x < bmax.x) {
    if (bmin.y <= point.y && point.y < bmax.y) {
      return true;
    }
  }
  return false;
}

Line2D::Line2D(const vec2 &_a, const vec2 &_b) : a(_a), b(_b) {
  aabb = AABB2D({a, b});
}

IntersectResult2D Line2D::intersect(const Ray2D &ray) const {

  IntersectResult2D result;

  vec2 bma = b - a;
  vec2 ame = a - ray.origin;
  float dxbma = cross(ray.direction, bma);
  float alpha = cross(ame, ray.direction) / dxbma;

  if (alpha >= 0.0f && alpha <= 1.0f) {
    float t = cross(ame, bma) / dxbma;
    if (t > 0.0f) {
      result.tEnter = t;
      result.tLeave = t;
      result.hit = true;
      result.normalEnter = normalize(vec2(a.y - b.y, b.x - a.x));
      result.normalLeave = result.normalEnter;
    }
  }
  return result;
}

vector<vec4> Line2D::lineRepresentation() const {
  return {{a.x, a.y, b.x, b.y}};
}

BoundingBox2D::BoundingBox2D(const vec2 &_bmin, const vec2 &_bmax) {
  aabb = AABB2D({_bmin, _bmax});
}

BoundingBox2D::BoundingBox2D(const vector<vec2> &points) {
  aabb = AABB2D(points);
}

BoundingBox2D::BoundingBox2D(const AABB2D &_aabb) { aabb = _aabb; }

BoundingBox2D::BoundingBox2D(const vector<AABB2D> &aabbs) {
  vector<vec2> points;
  for (const auto &_aabb : aabbs) {
    points.push_back(_aabb.bmin);
    points.push_back(_aabb.bmax);
  }
  aabb = AABB2D(points);
}

BoundingBox2D::BoundingBox2D(const vector<BoundingBox2D> &aabbs) {
  vector<vec2> points;
  for (const auto &_aabb : aabbs) {
    points.push_back(_aabb.aabb.bmin);
    points.push_back(_aabb.aabb.bmax);
  }
  aabb = AABB2D(points);
}

IntersectResult2D BoundingBox2D::intersect(const Ray2D &ray) const {
  IntersectResult2D ret;

  float tx1 = (aabb.bmin.x - ray.origin.x) / ray.direction.x;
  float tx2 = (aabb.bmax.x - ray.origin.x) / ray.direction.x;

  float tmin = glm::min(tx1, tx2);
  float tmax = glm::max(tx1, tx2);

  float ty1 = (aabb.bmin.y - ray.origin.y) / ray.direction.y;
  float ty2 = (aabb.bmax.y - ray.origin.y) / ray.direction.y;

  tmin = glm::max(tmin, glm::min(ty1, ty2));
  tmax = glm::min(tmax, glm::max(ty1, ty2));

  if (tmin > 0 && tmax >= tmin) {
    ret.hit = true;
    ret.tEnter = tmin;
    ret.tLeave = tmax;

    if (tmin == tx1) {
      ret.normalEnter = {-1.0f, 0.0f};
    } else if (tmin == tx2) {
      ret.normalEnter = {0.0f, 1.0f};
    } else if (tmin == ty1) {
      ret.normalEnter = {0.0f, -1.0f};
    } else if (tmin == ty2) {
      ret.normalEnter = {0.0f, 1.0f};
    }
    if (tmax == tx1) {
      ret.normalLeave = {-1.0f, 0.0f};
    } else if (tmax == tx2) {
      ret.normalLeave = {0.0f, 1.0f};
    } else if (tmax == ty1) {
      ret.normalLeave = {0.0f, -1.0f};
    } else if (tmax == ty2) {
      ret.normalLeave = {0.0f, 1.0f};
    }
  }

  return ret;
}

IntersectResult2D BoundingBox2D::intersectCheck(const Ray2D &ray) const {
  IntersectResult2D ret;

  float tx1 = (aabb.bmin.x - ray.origin.x) / ray.direction.x;
  float tx2 = (aabb.bmax.x - ray.origin.x) / ray.direction.x;

  float tmin = glm::min(tx1, tx2);
  float tmax = glm::max(tx1, tx2);

  float ty1 = (aabb.bmin.y - ray.origin.y) / ray.direction.y;
  float ty2 = (aabb.bmax.y - ray.origin.y) / ray.direction.y;

  tmin = glm::max(tmin, glm::min(ty1, ty2));
  tmax = glm::min(tmax, glm::max(ty1, ty2));

  if (tmin >= 0 && tmax >= tmin) {
    ret.hit = true;
    ret.tEnter = tmin;
    ret.tLeave = tmax;
  }

  return ret;
}

Sphere2D::Sphere2D(const vec2 &_center, float _radius, const vec2 &_facing,
                   float _maxAngle)
    : center(_center), radius(_radius), facing(normalize(_facing)),
      maxAngle(_maxAngle) {
  vector<vec2> points;

  if (abs(angle({-radius, 0.0f}, facing) < maxAngle / 2.0f))
    points.push_back({center.x - radius, center.y});
  if (abs(angle({radius, 0.0f}, facing) < maxAngle / 2.0f))
    points.push_back({center.x + radius, center.y});
  if (abs(angle({0.0f, -radius}, facing) < maxAngle / 2.0f))
    points.push_back({center.x, center.y - radius});
  if (abs(angle({0.0f, radius}, facing) < maxAngle / 2.0f))
    points.push_back({center.x, center.y + radius});

  points.push_back(rotate(radius * facing, -maxAngle / 2.0f) + center);
  points.push_back(rotate(radius * facing, maxAngle / 2.0f) + center);

  aabb = AABB2D(points);
}

IntersectResult2D Sphere2D::intersect(const Ray2D &ray) const {
  IntersectResult2D ret;

  float a = dot(ray.direction, ray.direction);
  float b = 2.0f * ray.direction.x * (ray.origin.x - center.x) +
            2.0f * ray.direction.y * (ray.origin.y - center.y);
  float c = dot(ray.origin - center, ray.origin - center) - radius * radius;

  float discriminante = sqrt(b * b - 4.0f * a * c);
  if (discriminante > 0.0f) {
    float tEnter = (-b - discriminante) / (2.0f * a);
    float tLeave = (-b + discriminante) / (2.0f * a);

    if (tEnter >= 0.0f) {
      vec2 enter = ray.origin + tEnter * ray.direction;
      vec2 leave = ray.origin + tLeave * ray.direction;
      vec2 normalEnter = normalize(enter - center);
      vec2 normalLeave = normalize(leave - center);
      if (abs(angle(normalEnter, facing)) <= maxAngle / 2.0f) {
        ret.tEnter = tEnter;
        ret.normalEnter = normalize(enter - center);
        ret.hit = true;
        if (abs(angle(normalLeave, facing)) <= maxAngle / 2.0f) {
          ret.tLeave = tLeave;
          ret.normalLeave = normalize(leave - center);
          return ret;
        } else {
          ret.tLeave = tEnter;
          ret.normalLeave = -normalize(enter - center);
          return ret;
        }
      }
      if (abs(angle(normalLeave, facing)) <= maxAngle / 2.0f) {
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
  float segmentAngle = maxAngle / 100;
  vec2 a = rotate(radius * facing, -maxAngle / 2.0f);
  for (int i = 0; i < 100; ++i) {
    vec2 b = rotate(a, segmentAngle);
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
