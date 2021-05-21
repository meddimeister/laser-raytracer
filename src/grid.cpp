#include "grid.h"
#include <iostream>

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin,
                                  const vec2 &_bmax) {
  vector<shared_ptr<Shape2D>> boxes;
  boxes.push_back(make_shared<BoundingBox2D>(_pos + _bmin, _pos + _bmax));
  return boxes;
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
