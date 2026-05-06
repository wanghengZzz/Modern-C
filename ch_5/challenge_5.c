#include "src/funcs.h"

int main(void) {
  complex double a = 0.123 + 2.1 * I;
  double sin_real = creal(do_sin(a));
  double sin_imagl = cimag(do_sin(a));
  double cos_real = creal(do_cos(a));
  double cos_imagl = cimag(do_cos(a));
  complex double sin_deri = get_derivatives(do_sin, a);
  complex double cos_deri = get_derivatives(do_cos, a);
  printf("sin real: %.20f + %.20f*I\n", sin_real, sin_imagl);
  printf("cos real: %.20f + %.20f*I\n", cos_real, cos_imagl);
  printf("derivatives of sin: %.20f + %.20f*I \n", creal(sin_deri),
         cimag(sin_deri));
  printf("derivatives of cos: %.20f + %.20f*I \n", creal(cos_deri),
         cimag(cos_deri));
  return 0;
}