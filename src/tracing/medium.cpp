#include "tracing/medium.h"
#include <iostream>

void Medium2D::action(Ray2D &ray, const IntersectResult2D &result,
                      vector<Ray2D> &createdRays) {
  actionEnter(ray, result);
  if (!ray.terminated) {
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
    if (transmitResult.hit) {
      internalResult = transmitResult;
    }

    actionTransmit(ray_transmit_in, internalResult);

    if (!ray_transmit_in.terminated) {
      ray_transmit_in.terminate(internalResult.tLeave);

      auto [ray_reflect_out, ray_transmit_out] = ray_transmit_in.refract(
          internalResult.tLeave, internalResult.normalLeave,
          sellmeier(_sellmeierCoeff, ray_transmit_in.wavelength), 1.0);

      auto reflectResult = intersect(ray_reflect_out);

      IntersectResult2D internalReflectResult;
      internalReflectResult.tEnter = 0.0;
      internalReflectResult.tLeave = 0.0;
      internalReflectResult.normalEnter = internalResult.normalEnter;
      internalReflectResult.normalLeave = -internalResult.normalEnter;
      internalReflectResult.hit = true;
      if (reflectResult.hit) {
        internalReflectResult = reflectResult;
      }

      actionTransmit(ray_reflect_out, internalReflectResult);
      if (!ray_reflect_out.terminated) {
        ray_reflect_out.terminate(internalReflectResult.tLeave);
      }
      createdRays.push_back(ray_reflect_out);

      if (!ray_transmit_out.terminated) {
        ray_transmit_out.origin += 20.0 * numeric_limits<double>::epsilon() *
                                   length(ray_transmit_out.origin) *
                                   ray_transmit_out.direction;
        createdRays.push_back(ray_transmit_out);
      }
    }
    createdRays.push_back(ray_reflect_in);
    createdRays.push_back(ray_transmit_in);
  }
}