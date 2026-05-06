#include "src/funcs.h"
#include "src/linalg.h"

int main(void) {
  complex double a = 2.0 + 0.12 * I;
  complex double x_0 = 0.25;
  size_t m_roots = 2;
  complex double res = Newton_solve(m_th_root_expr, a, x_0, m_roots);
  printf("root of %.3f + %.3f * I : %.20f + %.20f*I\n", creal(a), cimag(a),
         creal(res), cimag(res));
  /*
    test_cases {
      {2.0, 0.0, 1.0, -3.0, 2.0, 7.0},
      {1.0, -6.0, 11.0, -6.0},
      {1.0, 0.0, 0.0, 0.0, 0, -1.0},
      {1.0, 6.3, 1.2, 3.0, 2.0, -1.0},
      {0.0, 1.0, 0.0, 0.0, 0.0, -1.0},
      {6.42, -12.1, 2 + 3*I, 4 - 6*I, 12, -1 + 3.12*I}
    };
  */

  complex double poly[] = {2.0, 0, 1.0, -3.0, 2.0, 7.0};
  size_t n = ARRAY_SIZE(poly);
  complex double *ans = calloc_safe(n, sizeof(poly[0]));
  size_t n_ans =
      find_all_zeros_real_coef_polynomial(poly, ARRAY_SIZE(poly), ans);

  /*
    Test f(x) = .0, for all x belongs to eigvals set
  */

  for (size_t i = 0; i < n_ans; ++i) {
    printf("eigen val: %7.4f + %7.4f*I\n", creal(ans[i]), cimag(ans[i]));
    complex double tot = .0;
    for (size_t j = 0; j < n; ++j)
      tot += cpow(ans[i], n - j - 1) * poly[j];
    printf("Total: %7.4f + %7.4f*I\n", creal(tot), cimag(tot));
  }
  if (ans)
    free(ans);
  return 0;
}