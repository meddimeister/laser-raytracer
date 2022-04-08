#include "tracing/medium.h"
#include <iostream>

vector<shared_ptr<Shape2D>> Medium2D::build() {
    vector<shared_ptr<Shape2D>> shapes;
    shapes.push_back(make_shared<BoundingBox2D>(pos + dvec2(-0.1,-0.1), pos + dvec2(0.1, 0.1)));
    return shapes;
}

void Medium2D::action(Ray2D &ray, const IntersectResult2D &result,
                      vector<Ray2D> &createdRays) {
  _enterAction(ray, result);
  ray.terminate(result.tEnter);

  auto [ray_reflect_in, ray_transmit_in] =
      ray.refract(result.tEnter, result.normalEnter, 1.0,
                  sellmeier(_sellmeierCoeff, ray.wavelength));
  
  auto internalResult = intersect(ray_transmit_in);
  double tLeave = 0.0;
  dvec2 normalLeave = result.normalEnter;
  if(internalResult.hit){
    tLeave = internalResult.tLeave;
    normalLeave = internalResult.normalLeave;
  }

  ray_transmit_in.terminate(tLeave);
  auto [ray_reflect_out, ray_transmit_out] =
       ray_transmit_in.refract(tLeave, -normalLeave, sellmeier(_sellmeierCoeff, ray_transmit_in.wavelength), 1.0);

  createdRays.push_back(ray_reflect_in);
  createdRays.push_back(ray_transmit_in);
  //createdRays.push_back(ray_reflect_out);
  if(!ray_transmit_out.terminated){
    createdRays.push_back(ray_transmit_out);
    _leaveAction(ray_transmit_out);
  }
}