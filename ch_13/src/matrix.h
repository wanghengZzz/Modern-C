#ifndef __MATRIX_H_
#define __MATRIX_H_

#include <assert.h>
#include <stddef.h>
#include <tgmath.h>

#include "utils.h"

typedef void (*matrix_ops_t)(void *dest, void const *d1, void const *d2);
typedef int (*matrix_unary_pos_t)(void const *d1);
typedef void (*prt_item_fn_t)(void const *d);
typedef struct {
  size_t *dim_arr;
  size_t ndims;
  size_t type_size;
  matrix_ops_t mul_fn;
  matrix_ops_t div_fn;
  matrix_ops_t plus_fn;
  matrix_ops_t minus_fn;
  matrix_unary_pos_t is_zero;
  void *data;

} Matrix;

#define ALLOC_MATRIX(fill_val, _mul_fn, _div_fn, _plus_fn, _minus_fn,      \
                     _is_zero, args...)                                    \
  ALLOC_INIT(Matrix, 1,                                                    \
             {.dim_arr = ALLOC_INIT(size_t, PP_NARG(args), args),          \
              .ndims = PP_NARG(args),                                      \
              .type_size = sizeof(typeof(fill_val)),                       \
              .mul_fn = _mul_fn,                                           \
              .div_fn = _div_fn,                                           \
              .plus_fn = _plus_fn,                                         \
              .minus_fn = _minus_fn,                                       \
              .is_zero = _is_zero,                                         \
              .data = meminit(                                             \
                  &(typeof(fill_val)){fill_val}, sizeof(typeof(fill_val)), \
                  MATRIX_GET_NUM_ELES(((size_t[]){args}), PP_NARG(args)) * \
                      sizeof(typeof(fill_val)))})

#define ALLOC_MATRIX_DUP(mat, type, _mul_fn, _div_fn, _plus_fn, _minus_fn, \
                         _is_zero, args...)                                \
  ALLOC_INIT(Matrix, 1,                                                    \
             {.dim_arr = ALLOC_INIT(size_t, PP_NARG(args), args),          \
              .ndims = PP_NARG(args),                                      \
              .type_size = sizeof(type),                                   \
              .mul_fn = _mul_fn,                                           \
              .div_fn = _div_fn,                                           \
              .plus_fn = _plus_fn,                                         \
              .minus_fn = _minus_fn,                                       \
              .is_zero = _is_zero,                                         \
              .data = memdup(mat, MATRIX_GET_NUM_ELES(((size_t[]){args}),  \
                                                      PP_NARG(args)) *     \
                                      sizeof(type))})

#define MATRIX_DUP(mat)                                                        \
  ({                                                                           \
    Matrix *__m = memdup((mat), sizeof(Matrix));                               \
    __m->dim_arr =                                                             \
        memdup((mat)->dim_arr, (mat)->ndims * sizeof((mat)->ndims));           \
    __m->data = memdup(                                                        \
        (mat)->data,                                                           \
        (mat)->type_size * MATRIX_GET_NUM_ELES((mat)->dim_arr, (mat)->ndims)); \
    __m;                                                                       \
  })

#define MATRIX_GET_NUM_ELES(arr, n) matrix_get_num_eles(arr, n)

#define INIT_IDENTITY_MATRIX(n, mat)                \
  ({                                                \
    for (size_t __i = 0; __i < n; ++__i)            \
      for (size_t __j = 0; __j < n; ++__j)          \
        mat[__i * n + __j] = __i == __j ? 1.0 : .0; \
  })

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define MATRIX_GET_ELE(m, args...) \
  ((size_t *)(m)                   \
       ->data)[matrix_get_ele_idx(m, PP_NARG(args), ((size_t[]){args}))]

#define MATRIX_GET_IDX(m, args...) \
  matrix_get_ele_idx(m, PP_NARG(args), ((size_t[]){args}))

size_t matrix_get_num_eles(size_t const *const dims, size_t n);

size_t matrix_get_ele_idx(Matrix *m, size_t n, size_t *idx_arr);
void inverse(Matrix *in, Matrix *out);
void matrix_mul_dbl_cmplx(Matrix *m1, Matrix *m2, Matrix *out);
void print_matrix(Matrix *m, prt_item_fn_t prt_fn);
void _print_matrix_recursive(Matrix *m, size_t dim_idx, size_t offset,
                             prt_item_fn_t prt_fn);
void free_matrix(Matrix *m);

static inline void matrix_mul_ops_dbl_cmplx(void *dest, void const *d1, void const *d2) {
  CHECK_ERR(d1 && d2, "Access nullptr");
  *((complex double*)dest) = ((*(complex double  *)d1) * (*(complex double  *)d2));
}

static inline void matrix_div_ops_dbl_cmplx(void *dest, void const *d1, void const *d2) {
  CHECK_ERR(d1 && d2, "Access nullptr");
  *((complex double*)dest) = ((*(complex double  *)d1) / (*(complex double  *)d2));
}

static inline void matrix_plus_ops_dbl_cmplx(void *dest, void const *d1, void const *d2) {
  CHECK_ERR(d1 && d2, "Access nullptr");
  *((complex double*)dest) = ((*(complex double  *)d1) + (*(complex double  *)d2));
}

static inline void matrix_minus_ops_dbl_cmplx(void *dest, void const *d1, void const *d2) {
  CHECK_ERR(d1 && d2, "Access nullptr");
  *((complex double*)dest) = ((*(complex double  *)d1) - (*(complex double  *)d2));
}

static inline int matrix_is_zero_dbl_cmplx(void const *d) {
  return (*(complex double *)d) == .0;
}

#define MATRIX_DIV_SCALAR_INPLACE(m, type, scalar)                    \
  ({                                                                  \
    size_t __n_eles = MATRIX_GET_NUM_ELES((m)->dim_arr, (m)->ndims);  \
    for (size_t __i = 0; __i < __n_eles; __i++) {                     \
      type *__pos = CONTAINER_OFFSET((m)->data, (m)->type_size, __i); \
      *__pos /= scalar;                                               \
    }                                                                 \
  })

#define MATRIX_MUL_SCALAR_INPLACE(m, type, scalar)                \
  ({                                                              \
    size_t __n_eles = MATRIX_GET_NUM_ELES(m->dim_arr, m->ndims);  \
    for (size_t __i = 0; __i < __n_eles; __i++) {                 \
      type *__pos = CONTAINER_OFFSET(m->data, m->type_size, __i); \
      *__pos = *__pos * scalar;                                   \
    }                                                             \
  })

#define MATRIX_ADD_SCALAR_INPLACE(m, type, scalar)                \
  ({                                                              \
    size_t __n_eles = MATRIX_GET_NUM_ELES(m->dim_arr, m->ndims);  \
    for (size_t __i = 0; __i < __n_eles; __i++) {                 \
      type *__pos = CONTAINER_OFFSET(m->data, m->type_size, __i); \
      *__pos = *__pos + scalar;                                   \
    }                                                             \
  })

#define MATRIX_MINUS_SCALAR_INPLACE(m, type, scalar)              \
  ({                                                              \
    size_t __n_eles = MATRIX_GET_NUM_ELES(m->dim_arr, m->ndims);  \
    for (size_t __i = 0; __i < __n_eles; __i++) {                 \
      type *__pos = CONTAINER_OFFSET(m->data, m->type_size, __i); \
      *__pos = *__pos - scalar;                                   \
    }                                                             \
  })

static inline void print_item_double(void const *d) {
  printf("%7.4f", (*(double *)d));
}

static inline void print_item_dbl_cmplx(void const *d) {
  printf("%7.4f + %7.4f*I", creal(*(complex double *)d),
         cimag(*(complex double *)d));
}

#endif