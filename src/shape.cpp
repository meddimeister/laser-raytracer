#include "shape.h"

vector<IntersectResult2D> Line2D::intersect(vector<Ray2D> &rays) {
	vector<IntersectResult2D> results;
  for (auto &ray : rays) {
    results.push_back(intersect(ray));
  }
	return results;
}

vector<Ray2D> Line2D::reflect(vector<Ray2D> &rays){
	vector<Ray2D> reflections;
  for (auto &ray : rays) {
    IntersectResult2D intersectResult = intersect(ray);
		if(intersectResult.hit)
    	reflections.push_back(ray.reflect(intersectResult.t, intersectResult.normal));
  }
	return reflections;
}

ostream &operator<<(ostream &stream, const IntersectResult2D &intersectResult) {
  return stream << "IntersectResult2D: {t: " << intersectResult.t
                << ", hit: " << intersectResult.hit << ", normal: ["
                << intersectResult.normal.x << ", " << intersectResult.normal.y
                << "]"
                << "}";
}

ostream &operator<<(ostream &stream, const IntersectResult3D &intersectResult) {
  return stream << "IntersectResult3D: {t: " << intersectResult.t
                << ", hit: " << intersectResult.hit << ", normal: ["
                << intersectResult.normal.x << ", " << intersectResult.normal.y
                << ", " << intersectResult.normal.z << "]"
                << "}";
}

ostream &operator<<(ostream &stream, const Line2D &line) {
  return stream << "Line2D: {a: [" << line.a.x << ", " << line.a.y << "]"
                << ", b: [" << line.b.x << ", " << line.b.y << "]"
                << "}";
}
