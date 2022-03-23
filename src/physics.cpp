#include "physics.h"

double snellius(double theta_e, double n_e, double n_t) {
  double theta_t = asin(n_e / n_t * sin(theta_e));
  return theta_t;
}

tuple<double, double, double, double> fresnel(double theta_e, double theta_t) {
  if (theta_e + theta_t != 0.0) {
    double sin_emt = sin(theta_e - theta_t);
    double sin_ept = sin(theta_e + theta_t);
    double tan_emt = tan(theta_e - theta_t);
    double tan_ept = tan(theta_e + theta_t);

    double r_perp = (sin_emt * sin_emt) / (sin_ept * sin_ept);
    double r_para = (tan_emt * tan_emt) / (tan_ept * tan_ept);
    double t_perp = 1 - r_perp;
    double t_para = 1 - r_para;

    return {r_perp, r_para, t_perp, t_para};
  } else {
    return {0.0, 0.0, 1.0, 1.0};
  }
}
