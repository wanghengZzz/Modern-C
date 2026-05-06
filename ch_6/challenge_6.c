#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/matrix.h"
#include "src/utils.h"

void matrix_mul(Matrix *m1, Matrix *m2, Matrix *out) {
  for (size_t i = 0; i < m1->dim_arr[0]; ++i) {
    for (size_t j = 0; j < m2->dim_arr[1]; ++j) {
      double tot = .0;
      double tmp = .0;
      for (size_t k = 0; k < m1->dim_arr[1]; ++k) {
        void *m1_i_k =
            CONTAINER_OFFSET(m1->data, m1->type_size, MATRIX_GET_IDX(m1, i, k));
        void *m2_k_j =
            CONTAINER_OFFSET(m2->data, m2->type_size, MATRIX_GET_IDX(m2, k, j));
        m1->mul_fn(&tmp, m1_i_k, m2_k_j);
        tot += tmp;
      }
      void *out_i_j = CONTAINER_OFFSET(out->data, out->type_size,
                                       MATRIX_GET_IDX(out, i, j));
      memcpy(out_i_j, &tot, out->type_size);
    }
  }
}

void matrix_mul_ops(void* dest, void const *d1, void const *d2) {
  CHECK_ERR(d1 && d2, "Access nullptr");
  *((double*)dest) = ((*(double *)d1) * (*(double *)d2));
}

void matrix_div_ops(void* dest, void const *d1, void const *d2) {
  CHECK_ERR(d1 && d2, "Access nullptr");
  *((double*)dest) = ((*(double *)d1) / (*(double *)d2));
}

void matrix_plus_ops(void* dest, void const *d1, void const *d2) {
  CHECK_ERR(d1 && d2, "Access nullptr");
  *((double*)dest) = ((*(double *)d1) + (*(double *)d2));
}

void matrix_minus_ops(void* dest, void const *d1, void const *d2) {
  CHECK_ERR(d1 && d2, "Access nullptr");
  *((double*)dest) = ((*(double *)d1) - (*(double *)d2));
}

int matrix_is_zero(void const *d) { return (*(double *)d) == .0; }

void print_item_double(void const *d) { printf("%7.4f", (*(double *)d)); };

int main(void) {
  double fill_val = .0;
  double mat[] = {
      -1, 2, -1, 1, -2, 6, -2, 6, -6,
  };

  Matrix *inv_m =
      ALLOC_MATRIX(fill_val, matrix_mul_ops, matrix_div_ops, matrix_plus_ops,
                   matrix_minus_ops, matrix_is_zero, 3, 3);

  Matrix *m =
      ALLOC_MATRIX_DUP(mat, double, matrix_mul_ops, matrix_div_ops,
                       matrix_plus_ops, matrix_minus_ops, matrix_is_zero, 3, 3);
  INIT_IDENTITY_MATRIX(3, ((double *)inv_m->data));
  inverse(m, inv_m);

  Matrix *mul_res = MATRIX_DUP(inv_m);

  printf("\nMultiple of two matrices: (inv_m @ m) \n");
  matrix_mul(inv_m, m, mul_res);
  print_matrix(mul_res, print_item_double);

  printf("\nMultiple of two matrices: (m @ inv_m) \n");
  matrix_mul(m, inv_m, mul_res);
  print_matrix(mul_res, print_item_double);

  printf("\nInverse matrix of m: \n");
  print_matrix(inv_m, print_item_double);

#ifdef DEBUG_PRT_MATRIX
  Matrix *test_m = ALLOC_MATRIX(
      fill_val, double, matrix_mul_ops, matrix_div_ops, matrix_plus_ops,
      matrix_minus_ops, matrix_is_zero, 4, 5, 6, 3, 3);
  print_matrix(test_m);
#endif

  free_matrix(m);
  free_matrix(inv_m);
  free_matrix(mul_res);
  return 0;
}