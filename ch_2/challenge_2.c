#include "src/funcs.h"

int main(void) {
  double a = 0.123;
  double sin_real = do_sin(a);
  double cos_real = do_cos(a);
  double sin_deri = get_derivatives(do_sin, a);
  double cos_deri = get_derivatives(do_cos, a);
  printf("sin real: %.20f\n", sin_real);
  printf("cos real: %.20f\n", cos_real);
  printf("derivatives of sin: %.20f\n", sin_deri);
  printf("derivatives of cos: %.20f\n", cos_deri);
  return 0;
}