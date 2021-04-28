#pragma once

#include "math.h"
#include "shape.h"
#include <vector>

using namespace std;
using namespace glm;

struct Mirror2D {
  vec2 pos;
  vec2 opticalAxis;
  vector<Line2D> segments;

  Mirror2D(const vec2 &_pos, const vec2 &_opticalAxis,
           const function<float(float)> &&_shapeFunction, int _segments)
		: pos(_pos), opticalAxis(_opticalAxis) {
			for(int i = 0; i < _segments; ++i){
				vec2 ap = {i*(1.0f/_segments), _shapeFunction(i*(1.0f/_segments))};
				vec2 bp = {(i+1)*(1.0f/_segments), _shapeFunction((i+1)*(1.0f/_segments))};

				vec2 am = {-i*(1.0f/_segments), _shapeFunction(-i*(1.0f/_segments))};
				vec2 bm = {-(i+1)*(1.0f/_segments), _shapeFunction(-(i+1)*(1.0f/_segments))};
				
				float rotationAngle = orientedAngle({0.0f, 1.0f}, opticalAxis);

				ap = rotate(ap, rotationAngle);
				bp = rotate(bp, rotationAngle);

				am = rotate(am, rotationAngle);
				bm = rotate(bm, rotationAngle);

				ap += pos;
				bp += pos;

				am += pos;
				bm += pos;

				segments.push_back(Line2D(ap, bp));
				segments.push_back(Line2D(am, bm));
			}
		}

	vector<Ray2D> reflect(vector<Ray2D> &rays);
};
