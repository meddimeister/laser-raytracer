#pragma once

#include "math/allmath.h"
#include "math/physics.h"
#include <functional>
#include <limits>
#include <ostream>
#include <iostream>
#include <vector>

using namespace std;
using namespace glm;

struct Ray2D {
  dvec2 origin;
  dvec2 direction;
  double power;
  double wavelength;
  bool terminated = false;
  double terminatedAt;

  Ray2D(const dvec2 &_origin, const dvec2 &_direction, double _power,
        double _wavelength)
      : origin(_origin), direction(_direction), power(_power),
        wavelength(_wavelength) {}

  Ray2D reflect(double t, const dvec2 &normal) const {
    dvec2 ori = origin + t * direction;
    dvec2 dir = glm::reflect(direction, normal);
    ori += numeric_limits<double>::epsilon() * 20.0 * length(ori) * dir;
    return Ray2D(ori, dir, power, wavelength);
  }

  tuple<Ray2D, Ray2D> refract(double t, const dvec2 &normal, double n_e,
                              double n_t) const {
    
    double dott = dot(direction, normal);
    double sign = dott/abs(dott);

    dvec2 orientedNormal = sign * normal;
    double theta_e = orientedAngle(direction, orientedNormal);
    double theta_t = snellius(theta_e, n_e, n_t);

    Ray2D ray_reflect = reflect(t, normal);
    Ray2D ray_transmit(origin + t * direction, {0.0, 0.0}, 0.0, wavelength);

    //total internal reflection
    if(isnan(theta_t)){ 
      ray_reflect.power = power;
      ray_transmit.terminate(0.0);
    }
    else{
      auto [r_perp, r_para, t_perp, t_para] = fresnel(abs(theta_e), abs(theta_t));
      ray_reflect.power = (r_perp + r_para) / 2 * power;
      ray_transmit.direction = rotate(orientedNormal, -theta_t);
      ray_transmit.power = (t_perp + t_para) / 2 * power;
    }

    return {ray_reflect, ray_transmit};
  }

  void terminate(double t) {
    terminated = true;
    terminatedAt = t;
  }
};

ostream &operator<<(ostream &stream, const Ray2D &ray);
