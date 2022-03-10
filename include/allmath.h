#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "glm.hpp"
#include "gtx/rotate_vector.hpp"
#include "gtx/vector_angle.hpp"
#include "gtx/spline.hpp"

using namespace std;
using namespace glm;

inline float cross(const vec2 &a, const vec2 &b) {
  return a.x * b.y - a.y * b.x;
}

inline vec2 lerp(vec2 x, vec2 y, float t) {
  return x * (1.f - t) + y * t;
}

inline vec2 bezier(vec2 a, vec2 b, vec2 c, vec2 d, float t){
  vec2 aa = lerp(a,b,t);
  vec2 bb = lerp(b,c,t);
  vec2 cc = lerp(c,d,t);

  vec2 aaa = lerp(aa, bb, t);
  vec2 bbb = lerp(bb, cc, t);

  vec2 point = lerp(aaa, bbb, t);
  return point;
}
