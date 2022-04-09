#include "tracing/medium.h"
#include <iostream>

vector<shared_ptr<Shape2D>> Medium2D::build() {
    return {};
}

void Medium2D::action(Ray2D &ray, const IntersectResult2D &result,
                      vector<Ray2D> &createdRays) {
  actionEnter(ray, result);
  ray.terminate(result.tEnter);

  auto [ray_reflect_in, ray_transmit_in] =
      ray.refract(result.tEnter, result.normalEnter, 1.0,
                  sellmeier(_sellmeierCoeff, ray.wavelength));

  auto transmitResult = intersect(ray_transmit_in);

  IntersectResult2D internalResult;
  internalResult.tEnter = 0.0;
  internalResult.tLeave = 0.0;
  internalResult.normalEnter = result.normalEnter;
  internalResult.normalLeave = -result.normalEnter;
  internalResult.hit = true;
  if(transmitResult.hit){
    internalResult = transmitResult;
  }

  actionTransmit(ray_transmit_in, internalResult);  

  ray_transmit_in.terminate(internalResult.tLeave);
  
  auto [ray_reflect_out, ray_transmit_out] =
       ray_transmit_in.refract(internalResult.tLeave, internalResult.normalLeave, sellmeier(_sellmeierCoeff, ray_transmit_in.wavelength), 1.0);

  createdRays.push_back(ray_reflect_in);
  createdRays.push_back(ray_transmit_in);
  //createdRays.push_back(ray_reflect_out);
  if(!ray_transmit_out.terminated){
    createdRays.push_back(ray_transmit_out);
  }
}