#include "funcs.h"

#include <string.h>

#include "linalg.h"

double get_factorial(int x) {
  long long init = 1;
  do {
    init *= x;
  } while (--x);
  return 1.0 / init;
}

complex double m_th_root_expr(complex double z, complex double x_0, size_t m) {
  complex double tgt = 1.0;
  while (m--) {
    tgt *= x_0;
  }
  return tgt - z;
}

complex double Newton_solve(Newton_method_t fn, complex double z,
                            complex double x_0, size_t m) {
  complex double deri, res;
  do {
    deri = get_m_roots_derivatives(fn, z, x_0, m);
    res = fn(z, x_0, m);
    x_0 = x_0 - res / deri;
    printf("Newton: %.10f + %.10f*I\n", creal(res), cimag(res));
  } while (fabs(creal(res)) > 1e-8 || fabs(cimag(res)) > 1e-8);
  return x_0;
}

int find_all_zeros_real_coef_polynomial(complex double *coef, size_t n,
                                        complex double *out) {
  size_t real_n_r = 0, real_n_l = 0;
  for (real_n_r = n - 1; real_n_r > 0 && fabs(creal(coef[real_n_r])) < 1e-9 &&
                         fabs(cimag(coef[real_n_r])) < 1e-9;
       --real_n_r);
  for (real_n_l = 0; real_n_l < n && fabs(creal(coef[real_n_l])) < 1e-9 &&
                     fabs(cimag(coef[real_n_l])) < 1e-9;
       ++real_n_l);

  size_t real_n = real_n_r - real_n_l + 1;
  size_t trailing_zeros = n - real_n_r - 1;
  if (real_n < 2) return 0;
  Matrix *ones = LINALG_ONES(
      complex double, matrix_mul_ops_dbl_cmplx, matrix_div_ops_dbl_cmplx,
      matrix_plus_ops_dbl_cmplx, matrix_minus_ops_dbl_cmplx,
      matrix_is_zero_dbl_cmplx, real_n - 2);
  Matrix *dia = LINALG_DIAG(ones, complex double, -1);
  for (size_t j = 1; j < real_n; ++j)
    memcpy(CONTAINER_OFFSET(dia->data, dia->type_size,
                            MATRIX_GET_IDX(dia, 0, j - 1)),
           &(complex double){-coef[real_n_l + j] / coef[real_n_l]},
           dia->type_size);
  Matrix *eigvals = linalg_eigvals(dia);
  // print_matrix(eigvals, print_item_dbl_cmplx);
  size_t nonzeros = MATRIX_GET_NUM_ELES(eigvals->dim_arr, eigvals->ndims);
  memcpy(out, eigvals->data, nonzeros * eigvals->type_size);

  putc('\n', stdout);
  // trailing_zeros
  size_t n_ans = nonzeros + trailing_zeros;
  for (size_t i = nonzeros; i < n_ans; ++i) out[i] = 0.0;
  free_matrix(ones);
  free_matrix(dia);
  free_matrix(eigvals);
  return n_ans;
}
