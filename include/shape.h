#pragma once

#include "math.h"
#include "ray.h"

struct IntersectResult2D {
  float tEnter;
  float tLeave;
  bool hit = false;
  vec2 normalEnter;
  vec2 normalLeave;
};

struct IntersectResult3D {
  float tEnter;
  float tLeave;
  bool hit = false;
  vec3 normalEnter;
  vec3 normalLeave;
};

struct AABB2D {
	vec2 bmin, bmax;
  AABB2D(const vector<vec2> &points) {
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
};

class Shape2D {
public:
  AABB2D aabb = AABB2D({{0.0f, 0.0f}, {0.0f, 0.0f}});

  virtual IntersectResult2D intersect(Ray2D &ray) const {
    return IntersectResult2D();
  }
  virtual vector<vec4> lineRepresentation() const { return {}; }
};

class Shape3D {
public:
  virtual IntersectResult3D intersect(Ray3D &ray) = 0;
};

class Line2D : public Shape2D {
public:
  vec2 a, b;

	Line2D(const vec2 &_a, const vec2 &_b) : a(_a), b(_b) {
		aabb = AABB2D({a, b});
	}

  inline IntersectResult2D intersect(Ray2D &ray) const {

    IntersectResult2D result;

    vec2 bma = b - a;
    vec2 ame = a - ray.origin;
    float dxbma = cross(ray.direction, bma);
    float alpha = cross(ame, ray.direction) / dxbma;

    if (alpha >= 0.0f && alpha <= 1.0f) {
      float t = cross(ame, bma) / dxbma;
      if (t >= 0.0f) {
        result.tEnter = t;
        result.tLeave = t;
        result.hit = true;
        result.normalEnter = normalize(vec2(a.y - b.y, b.x - a.x));
        result.normalLeave = result.normalEnter;
      }
    }
    return result;
  }

  vector<vec4> lineRepresentation() const;
};

class BoundingBox2D : public Shape2D {
public:
	BoundingBox2D(const vec2 &_bmin, const vec2 &_bmax) {
		aabb = AABB2D({_bmin, _bmax});
	}

	BoundingBox2D(const vector<vec2> &points) {
		aabb = AABB2D(points);
	}

	BoundingBox2D(const AABB2D &_aabb) {
		aabb = _aabb;
	}

	BoundingBox2D(const vector<AABB2D> &aabbs) {
		vector<vec2> points;
		for(const auto &_aabb : aabbs){
			points.push_back(_aabb.bmin);
			points.push_back(_aabb.bmax);
		}
		aabb = AABB2D(points);
	}

	BoundingBox2D(const vector<BoundingBox2D> &aabbs) {
		vector<vec2> points;
		for(const auto &_aabb : aabbs){
			points.push_back(_aabb.aabb.bmin);
			points.push_back(_aabb.aabb.bmax);
		}
		aabb = AABB2D(points);
	}

  inline IntersectResult2D intersect(Ray2D &ray) const{
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

  inline IntersectResult2D
  intersectCheck(Ray2D &ray) const{
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

  vector<vec4> lineRepresentation() const;
};

ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult);
ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult);
ostream &operator<<(ostream &stream, const Line2D &line);
