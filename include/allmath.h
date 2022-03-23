#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "glm.hpp"
#include "gtx/rotate_vector.hpp"
#include "gtx/vector_angle.hpp"
#include "gtx/spline.hpp"

using namespace std;
using namespace glm;

inline double cross(const dvec2 &a, const dvec2 &b) {
  return a.x * b.y - a.y * b.x;
}

inline dvec2 lerp(dvec2 x, dvec2 y, double t) {
  return x * (1. - t) + y * t;
}

inline dvec2 bezier(dvec2 a, dvec2 b, dvec2 c, dvec2 d, double t){
  dvec2 aa = lerp(a,b,t);
  dvec2 bb = lerp(b,c,t);
  dvec2 cc = lerp(c,d,t);

  dvec2 aaa = lerp(aa, bb, t);
  dvec2 bbb = lerp(bb, cc, t);

  dvec2 point = lerp(aaa, bbb, t);
  return point;
}
