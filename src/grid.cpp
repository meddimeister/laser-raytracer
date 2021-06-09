#include "grid.h"
#include <iostream>

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin,
                                  const vec2 &_bmax) {
  vector<shared_ptr<Shape2D>> boxes;
  boxes.push_back(make_shared<BoundingBox2D>(_pos + _bmin, _pos + _bmax));
  return boxes;
}

void Grid2D::action(Ray2D &ray, const IntersectResult2D &result,
                    vector<Ray2D> &createdRays) {
  vec2 enter = ray.origin + result.tEnter * ray.direction;

  float xf = (enter.x - cornerMin.x) / dx;
  float yf = (enter.y - cornerMin.y) / dy;

  int x = xf;
  int y = yf;

  if (x == maxX && ray.direction.x < 0.0f) {
    x = x - 1;
  }

  if (y == maxY && ray.direction.y < 0.0f) {
    y = y - 1;
  }

  int stepX = ray.direction.x >= 0.0f ? 1 : -1;
  int stepY = ray.direction.y >= 0.0f ? 1 : -1;

  int nextX = stepX > 0 ? x + stepX : x;
  int nextY = stepY > 0 ? y + stepY : y;

  float tMaxX = (cornerMin.x + nextX * dx - ray.origin.x) / ray.direction.x;
  float tMaxY = (cornerMin.y + nextY * dy - ray.origin.y) / ray.direction.y;

  float tDeltaX = dx / abs(ray.direction.x);
  float tDeltaY = dy / abs(ray.direction.y);

  float tLast = result.tEnter;

  while (x >= 0 && x < maxX && y >= 0 && y < maxY) {
    float tTravel;
    float xCurrent = x;
    float yCurrent = y;
    if (tMaxX < tMaxY) {
      tTravel = tMaxX - tLast;
      tLast = tMaxX;
      tMaxX = tMaxX + tDeltaX;
      x = x + stepX;
    } else {
      tTravel = tMaxY - tLast;
      tLast = tMaxY;
      tMaxY = tMaxY + tDeltaY;
      y = y + stepY;
    }
    float distance = length(tTravel * ray.direction);
		cellAction(ray, distance, data[yCurrent * maxX + xCurrent]);
  }

  ACTION_PRESETS::pass(ray, result, createdRays);
}

float Grid2D::sum(){
	float sum = 0.0f;
	for (const auto &cell : data){
		sum += cell;
	}
	return sum;
}

ostream &operator<<(ostream &stream, const Grid2D &grid) {
  cout << "Grid2D: {grid: [" << endl;
  for (int y = 0; y < grid.maxY; ++y) {
    for (int x = 0; x < grid.maxX; ++x) {
      cout << grid.data[y * grid.maxX + x] << "\t";
    }
    cout << endl;
  }
  return cout << "]}";
}
