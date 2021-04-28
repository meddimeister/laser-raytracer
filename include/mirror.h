#pragma once

#include "math.h"
#include <vector>

using namespace std;
using namespace glm;

struct Mirror {
  vector<vec2> vertices;
  vector<vec2> indices;

  Mirror(const vector<vec2> &&_vertices, const vector<vec2> &&_indices)
      : vertices(_vertices), indices(_indices) {}
}
