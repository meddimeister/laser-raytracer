#include "physics.h"

float snellius(float theta_e, float n_e, float n_t)
{
  float theta_t = asin(n_e / n_t * sin(theta_e));
  return theta_t;
}

tuple<float, float, float, float> fresnel(float theta_e, float theta_t)
{
  if (theta_e + theta_t != 0.0f)
  {
    float sin_emt = sin(theta_e - theta_t);
    float sin_ept = sin(theta_e + theta_t);
    float tan_emt = tan(theta_e - theta_t);
    float tan_ept = tan(theta_e + theta_t);

    float r_perp = (sin_emt * sin_emt) / (sin_ept * sin_ept);
    float r_para = (tan_emt * tan_emt) / (tan_ept * tan_ept);
    float t_perp = 1 - r_perp;
    float t_para = 1 - r_para;

    return {r_perp, r_para, t_perp, t_para};
  }
  else
  {
    return {0.0f, 0.0f, 1.0f, 1.0f};
  }
}
