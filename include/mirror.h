#pragma once

#include "math.h"
#include "shape.h"
#include "object.h"
#include <vector>
#include <memory>

using namespace std;
using namespace glm;

class Mirror2D : public Object2D {
public:
  Mirror2D(const vec2 &_pos, const vec2 &_opticalAxis,
           const function<float(float)> &&_shapeFunction, int _segments)
		: Object2D(_pos, _opticalAxis) {
			for(int i = 0; i < _segments; ++i){
				vec2 ap = {i*(1.0f/_segments), _shapeFunction(i*(1.0f/_segments))};
				vec2 bp = {(i+1)*(1.0f/_segments), _shapeFunction((i+1)*(1.0f/_segments))};

				vec2 am = {-i*(1.0f/_segments), _shapeFunction(-i*(1.0f/_segments))};
				vec2 bm = {-(i+1)*(1.0f/_segments), _shapeFunction(-(i+1)*(1.0f/_segments))};
				
				float rotationAngle = orientedAngle({0.0f, 1.0f}, _opticalAxis);

				ap = rotate(ap, rotationAngle);
				bp = rotate(bp, rotationAngle);

				am = rotate(am, rotationAngle);
				bm = rotate(bm, rotationAngle);

				ap += pos;
				bp += pos;

				am += pos;
				bm += pos;

				shapes.push_back(make_shared<Line2D>(ap, bp));
				shapes.push_back(make_shared<Line2D>(am, bm));
			}
		}
};
