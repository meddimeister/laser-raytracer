#pragma once

#include "allmath.h"
#include "physics.h"
#include <functional>
#include <limits>
#include <ostream>
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
    ori += numeric_limits<double>::epsilon() * 2.0 * length(ori) * dir;
    return Ray2D(ori, dir, power, wavelength);
  }

  tuple<Ray2D, Ray2D> refract(double t, const dvec2 &normal, double n_e,
                              double n_t) const {
    dvec2 orientedNormal = -normal;
    double theta_e = orientedAngle(direction, orientedNormal);
    double theta_t = snellius(theta_e, n_e, n_t);
    auto [r_perp, r_para, t_perp, t_para] = fresnel(abs(theta_e), abs(theta_t));
    Ray2D ray_reflect = reflect(t, normal);
    ray_reflect.power = (r_perp + r_para) / 2 * power;

    dvec2 transmit_dir = rotate(orientedNormal, -theta_t);
    double transmit_power = (t_perp + t_para) / 2 * power;
    Ray2D ray_transmit(origin + t * direction, transmit_dir, transmit_power,
                       wavelength);

    return {ray_reflect, ray_transmit};
  }

  void terminate(double t) {
    terminated = true;
    terminatedAt = t;
  }
};

ostream &operator<<(ostream &stream, const Ray2D &ray);
