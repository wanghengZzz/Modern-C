
#include "matrix.h"

#include <stdio.h>
size_t matrix_get_ele_idx(Matrix *m, size_t n, size_t *idx_arr) {
  size_t idx = 0;
  size_t pre_mul = 1;
  assert(n == m->ndims);

  for (int i = (int)n - 1; i >= 0; i--) {
    size_t curr_dim = idx_arr[i];
    idx += curr_dim * pre_mul;
    pre_mul *= m->dim_arr[i];
  }
  return idx;
}

size_t matrix_get_num_eles(size_t const *const dims, size_t n) {
  size_t num_eles = 1;
  for (size_t i = 0; i < n; ++i) num_eles *= dims[i];
  return num_eles;
}

void inverse(Matrix *in, Matrix *out) {
  if (!in || in->ndims != 2 || in->dim_arr[0] != in->dim_arr[1]) {
    out = NULL;
    return;
  }

  void *dest = out->data;

  char mat[in->type_size * MATRIX_GET_NUM_ELES(in->dim_arr, in->ndims)];
  memcpy(mat, in->data,
         in->type_size * MATRIX_GET_NUM_ELES(in->dim_arr, in->ndims));

  int n = in->dim_arr[0];
  int k_flags[n];
  int order[n];

  memset(k_flags, 0, sizeof(k_flags));
  char tmp[in->type_size];
  char tmp2[in->type_size];
  char pivot[in->type_size];
  for (int k = 0; k < n; ++k) {
    int ak = -1;
    for (int i = 0; i < n; ++i) {
      if (!k_flags[i] && !in->is_zero(CONTAINER_OFFSET(
                             mat, in->type_size, MATRIX_GET_IDX(in, i, k)))) {
        ak = i;
        order[k] = i;
        k_flags[i] = 1;
        break;
      }
    }
    memcpy(pivot,
           CONTAINER_OFFSET(mat, in->type_size, MATRIX_GET_IDX(in, ak, k)),
           in->type_size);
    if (ak < 0) {
      printf("Error: Matrix is singular, cannot find inverse.\n");
      return;
    }
    char ops_res[in->type_size];
    for (int i = 0; i < n; ++i) {
      void *mat_ak_i =
          CONTAINER_OFFSET(mat, in->type_size, MATRIX_GET_IDX(in, ak, i));
      void *dest_ak_i =
          CONTAINER_OFFSET(dest, out->type_size, MATRIX_GET_IDX(out, ak, i));
      in->div_fn(ops_res, mat_ak_i, pivot);
      memcpy(mat_ak_i, &ops_res, in->type_size);
      in->div_fn(ops_res, dest_ak_i, pivot);
      memcpy(dest_ak_i, &ops_res, in->type_size);
    }
    for (int i = 0; i < n; ++i) {
      if (i != ak) {
        void *mat_i_k =
            CONTAINER_OFFSET(mat, in->type_size, MATRIX_GET_IDX(in, i, k));
        memcpy(tmp, mat_i_k, in->type_size);
        for (int j = 0; j < n; ++j) {
          void *mat_i_j =
              CONTAINER_OFFSET(mat, in->type_size, MATRIX_GET_IDX(in, i, j));
          void *dest_i_j =
              CONTAINER_OFFSET(dest, out->type_size, MATRIX_GET_IDX(in, i, j));
          void *mat_ak_j =
              CONTAINER_OFFSET(mat, in->type_size, MATRIX_GET_IDX(in, ak, j));
          void *dest_ak_j =
              CONTAINER_OFFSET(dest, out->type_size, MATRIX_GET_IDX(in, ak, j));
          in->mul_fn(ops_res, mat_ak_j, tmp);
          memcpy(tmp2, ops_res, in->type_size);
          in->minus_fn(ops_res, mat_i_j, tmp2);
          memcpy(mat_i_j, ops_res, in->type_size);
          in->mul_fn(ops_res, dest_ak_j, tmp);
          memcpy(tmp2, ops_res, in->type_size);
          out->minus_fn(ops_res, dest_i_j, tmp2);
          memcpy(dest_i_j, ops_res, out->type_size);
        }
      }
    }
  }
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      void *mat_i_j =
          CONTAINER_OFFSET(mat, in->type_size, MATRIX_GET_IDX(in, i, j));
      void *dest_order_j = CONTAINER_OFFSET(dest, out->type_size,
                                            MATRIX_GET_IDX(out, order[i], j));
      memcpy(mat_i_j, dest_order_j, in->type_size);
    }
  }
  memcpy(dest, mat,
         in->type_size * MATRIX_GET_NUM_ELES(in->dim_arr, in->ndims));
}

void _print_matrix_recursive(Matrix *m, size_t dim_idx, size_t offset,
                             prt_item_fn_t prt_fn) {
  size_t n = m->dim_arr[dim_idx];

  if (dim_idx == m->ndims - 1) {
    putchar('[');
    for (size_t i = 0; i < n; i++) {
      void *ptr = (char *)m->data + (offset + i) * m->type_size;

      prt_fn(ptr);

      if (i < n - 1) printf(", ");
    }
    putchar(']');
  } else {
    putchar('[');

    size_t stride = 1;
    for (size_t j = dim_idx + 1; j < m->ndims; j++) {
      stride *= m->dim_arr[j];
    }

    for (size_t i = 0; i < n; i++) {
      _print_matrix_recursive(m, dim_idx + 1, offset + i * stride, prt_fn);

      if (i < n - 1) {
        printf(",\n");
        for (size_t s = 0; s <= dim_idx; s++) printf(" ");
      }
    }
    putchar(']');
  }
}

void print_matrix(Matrix *m, prt_item_fn_t prt_fn) {
  if (!m || !m->data) {
    printf("None\n");
    return;
  }
  printf("tensor(");
  _print_matrix_recursive(m, 0, 0, prt_fn);
  printf(", shape=(");
  for (size_t i = 0; i < m->ndims; i++) {
    printf("%zu%s", m->dim_arr[i], (i == m->ndims - 1) ? "" : ", ");
  }
  printf("))\n");
}

void free_matrix(Matrix *m) {
  if (!m) return;
  if (m->data) free(m->data);
  if (m->dim_arr) free(m->dim_arr);
  free(m);
}
