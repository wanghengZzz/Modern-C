#ifndef __FUNCS_H_
#define __FUNCS_H_

#include <math.h>
#include <stdlib.h>
#include <tgmath.h>

#include "utils.h"

#ifdef __STDC_NO_COMPLEX__
#error "we need compelx arithemic"
#endif

#define sin_dx 1e-7
#define derivatives_h 1e-9
typedef complex double (*deri_t)(complex double);

#define s5 -0.00000002505210838544
#define s4 0.00000275573192239859
#define s3 -0.00019841269841269841
#define s2 0.00833333333333333322
#define s1 -0.16666666666666665741

#define c5 -0.00000027557319223986
#define c4 0.00002480158730158730
#define c3 -0.00138888888888888894
#define c2 0.04166666666666666435
#define c1 -0.50000000000000000000

#define POLYNOMIAL2_ODD(xx) \
  ((((s5 * (xx) + s4) * (xx) + s3) * (xx) + s2) * (xx))
#define POLYNOMIAL_ODD(xx) (POLYNOMIAL2_ODD(xx) + s1)
#define TAYLOR_SIN(xx, x)                                                \
  ({                                                                     \
    complex double __tmp =                                               \
        ((POLYNOMIAL_ODD(xx) * (x) - 0.5 * (sin_dx)) * (xx) + (sin_dx)); \
    complex double __res = (x) + __tmp;                                  \
    __res;                                                               \
  })

#define POLYNOMIAL2_EVEN(xx) \
  (((((c5 * (xx) + c4) * (xx) + c3) * (xx) + c2) * (xx) + c1) * (xx))

#define TAYLOR_COS(xx, x)                                                  \
  ({                                                                       \
    complex double __res = (1.0 + POLYNOMIAL2_EVEN(xx)) -                  \
                           ((POLYNOMIAL_ODD(xx) * (x)) * (xx) * (sin_dx)); \
    __res;                                                                 \
  })

static inline double du_dx(deri_t func, double x, double y, double h) {
  return (creal(func((x + h) + y * I)) - creal(func((x - h) + y * I))) /
         (2 * h);
}

static inline double du_dy(deri_t func, double x, double y, double h) {
  return (creal(func(x + (y + h) * I)) - creal(func(x + (y - h) * I))) /
         (2 * h);
}

static inline double dv_dx(deri_t func, double x, double y, double h) {
  return (cimag(func((x + h) + y * I)) - cimag(func((x - h) + y * I))) /
         (2 * h);
}

static inline double dv_dy(deri_t func, double x, double y, double h) {
  return (cimag(func(x + (y + h) * I)) - cimag(func(x + (y - h) * I))) /
         (2 * h);
}

static inline complex double do_sin(complex double x) {
  /*

      The Taylor series of a real or complex-valued function f (x),
      that is infinitely differentiable at a real or complex number "a", is the
      power series f(x) = f(a) + f'(x) / 1! (x - a) + f'(x) / 2! (x - a) ^ 2 +
      f'(x) / 3! (x - a) ^ 3 + ...

      Argument Reduction
      X approximately equals to x + dx
      sin(x + dx) = sin(x)cos(dx) + sin(dx)cos(x)
      cos(dx) approximately equals to 1
      sin(dx) approximately equals to dx
      sin(x + dx) = sin(x) + dx * cos(x)
      sin(x) = x - x^3/3! + x^5/5! - x^7/7! + ...
      cos(x) = 1 - x^2/2! + ...
      sin(x + dx) = x - x^3/3! + x^5/5! - x^7/7! + ... + dx * (1 - x^2/2!)

      The computed polynomial is a variation of the Taylor series expansion for
      sin(x + dx) = x - x^3/3! + x^5/5! - x^7/7! + x^9/9! - dx*x^2/2 + dx
  */
  return TAYLOR_SIN(x * x, x);
}

static inline complex double do_cos(complex double x) {
  /*
      cos(x + dx) = cos(x)cos(dx) - sin(x)sin(dx)
      cos(x + dx) = cos(x) - dx * (x - x^3/3! + x^5/5! - x^7/7! + ...)
      cos(x + dx) = 1 - x^2/2! + ... - dx * (x - x^3/3! + x^5/5! - x^7/7! + ...)
      cos(x + dx) = 1

      we also know sin'(x) equals to cos(x)
  */
  // return get_derivatives(do_sin, x);
  return TAYLOR_COS(x * x, x);
}

static inline complex double get_derivatives(deri_t func, complex double z) {
  if (cimag(z) < 1e-9) goto CAL_DERI;

  double ux = du_dx(func, creal(z), cimag(z), creal(derivatives_h));
  double uy = du_dy(func, creal(z), cimag(z), creal(derivatives_h));
  double vx = dv_dx(func, creal(z), cimag(z), creal(derivatives_h));
  double vy = dv_dy(func, creal(z), cimag(z), creal(derivatives_h));

#ifdef DEBUG
  printf("=== function at z = %.3f + %.3fi ===\n", creal(z), cimag(z));
  printf("  du/dx = %+.10f,  dv/dy = %+.10f  => diff = %.2e\n", ux, vy,
         fabs(ux - vy));
  printf("  du/dy = %+.10f, -dv/dx = %+.10f  => diff = %.2e\n", uy, -vx,
         fabs(uy + vx));
#endif

  CHECK_ERR(fabs(ux - vy) < 1e-5 && fabs(uy + vx) < 1e-5,
            "Cauchy-Riemann: NOT SATISFIED");
CAL_DERI:
  complex double h = derivatives_h + 1.0 * derivatives_h * (cimag(z) > 0);
  return (func(z + h) - func(z - h)) / (2.0 * h);
}

double get_factorial(int x);

#endif