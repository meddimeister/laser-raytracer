#include "grid.h"
#include <iostream>

vector<shared_ptr<Shape2D>> build(const vec2 &_pos, const vec2 &_bmin,
                                  const vec2 &_bmax) {
  vector<shared_ptr<Shape2D>> boxes;
  // float num_x = 100;
  // float num_y = 100;
  // float width = (_bmax.x - _bmin.x)/num_x;
  // float height = (_bmax.y - _bmin.y)/num_y;
  // for(unsigned int y = 0; y < num_y; ++y){
  //	for(unsigned int x = 0; x < num_x; ++x){
  //		vec2 cellmin = {_bmin.x + x*width, _bmin.y + y*height};
  //		vec2 cellmax = {_bmin.x + (x+1)*width, _bmin.y + (y+1)*height};
  //		boxes.push_back(make_shared<BoundingBox2D>(_pos + cellmin, _pos
  //+ cellmax));
  //	}
  //}
  boxes.push_back(make_shared<BoundingBox2D>(_pos + _bmin, _pos + _bmax));
  return boxes;
}

ostream &operator<<(ostream &stream, const Grid2D &grid) {
  cout << "Grid2D: {grid: [" << endl;
  for (int y = 0; y < grid.maxY; ++y) {
    for (int x = 0; x < grid.maxX; ++x) {
      cout << grid.grid[y * grid.maxX + x] << "\t";
    }
    cout << endl;
  }
  return cout << "]}";
}
