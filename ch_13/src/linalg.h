#ifndef __LINALG_H_
#define __LINALG_H_

#include "funcs.h"
#include "matrix.h"

#define LINALG_ONES(type, _mul_fn, _div_fn, _plus_fn, _minus_fn, _is_zero,     \
                    ...)                                                       \
  ALLOC_MATRIX(((type){1.0}), _mul_fn, _div_fn, _plus_fn, _minus_fn, _is_zero, \
               __VA_ARGS__)

#define LINALG_ZEROS(type, _mul_fn, _div_fn, _plus_fn, _minus_fn, _is_zero,    \
                     ...)                                                      \
  ALLOC_MATRIX(((type){0.0}), _mul_fn, _div_fn, _plus_fn, _minus_fn, _is_zero, \
               __VA_ARGS__)

#define LINALG_DIAG(m, type, bias)                                            \
  ({                                                                          \
    size_t __n_eles = MATRIX_GET_NUM_ELES((m)->dim_arr, (m)->ndims);          \
    size_t __abs_bias = (size_t)abs((int)(bias));                             \
    size_t __dim = __n_eles + __abs_bias;                                     \
    Matrix *__m =                                                             \
        ALLOC_MATRIX((type){0}, (m)->mul_fn, (m)->div_fn, (m)->plus_fn,       \
                     (m)->minus_fn, (m)->is_zero, __dim, __dim);              \
    size_t __bias_row = (bias) < 0 ? __abs_bias : 0;                          \
    size_t __bias_col = (bias) > 0 ? __abs_bias : 0;                          \
    for (size_t __i = 0; __i < __n_eles; ++__i)                               \
      memcpy(CONTAINER_OFFSET(__m->data, __m->type_size,                      \
                              (__i + __bias_row) * __dim + __bias_col + __i), \
             CONTAINER_OFFSET((m)->data, (m)->type_size, __i),                \
             (m)->type_size);                                                 \
    __m;                                                                      \
  })

#define LINALG_IDENTITY(n, type, _mul_fn, _div_fn, _plus_fn, _minus_fn,   \
                        _is_zero)                                         \
  ({                                                                      \
    Matrix *__m = ALLOC_MATRIX(((type){0.0}), _mul_fn, _div_fn, _plus_fn, \
                               _minus_fn, _is_zero, (n), (n));            \
    for (size_t __i = 0; __i < (n); ++__i)                                \
      memcpy(CONTAINER_OFFSET(__m->data, __m->type_size,                  \
                              MATRIX_GET_IDX(__m, __i, __i)),             \
             &(type){1.0}, __m->type_size);                               \
    __m;                                                                  \
  })

#define LINALG_Q(m, type, n, _i, _j, _mul_fn, _div_fn, _plus_fn, _minus_fn, \
                 _is_zero)                                                  \
  ({                                                                        \
    Matrix *__lq_m = LINALG_IDENTITY(n, type, _mul_fn, _div_fn, _plus_fn,   \
                                     _minus_fn, _is_zero);                  \
    type __lq_xi = *(type *)CONTAINER_OFFSET((m)->data, (m)->type_size,     \
                                             MATRIX_GET_IDX(m, _j, _j));    \
    type __lq_xj = *(type *)CONTAINER_OFFSET((m)->data, (m)->type_size,     \
                                             MATRIX_GET_IDX(m, _i, _j));    \
    double __lq_r_sq =                                                      \
        creal(__lq_xi) * creal(__lq_xi) + cimag(__lq_xi) * cimag(__lq_xi) + \
        creal(__lq_xj) * creal(__lq_xj) + cimag(__lq_xj) * cimag(__lq_xj);  \
    double __lq_r = sqrt(__lq_r_sq);                                        \
    type __lq_c = conj(__lq_xi) / __lq_r;                                   \
    type __lq_s = conj(__lq_xj) / __lq_r;                                   \
    memcpy(CONTAINER_OFFSET(__lq_m->data, __lq_m->type_size,                \
                            MATRIX_GET_IDX(__lq_m, _i, _i)),                \
           &(type){conj(__lq_c)}, __lq_m->type_size);                       \
    memcpy(CONTAINER_OFFSET(__lq_m->data, __lq_m->type_size,                \
                            MATRIX_GET_IDX(__lq_m, _j, _j)),                \
           &(type){__lq_c}, __lq_m->type_size);                             \
    memcpy(CONTAINER_OFFSET(__lq_m->data, __lq_m->type_size,                \
                            MATRIX_GET_IDX(__lq_m, _i, _j)),                \
           &(type){-conj(__lq_s)}, __lq_m->type_size);                      \
    memcpy(CONTAINER_OFFSET(__lq_m->data, __lq_m->type_size,                \
                            MATRIX_GET_IDX(__lq_m, _j, _i)),                \
           &(type){__lq_s}, __lq_m->type_size);                             \
    __lq_m;                                                                 \
  })

#define LINALG_CONJ_T_INPLACE(m, type)                                     \
  ({                                                                       \
    size_t __n = (m)->dim_arr[0];                                          \
    for (size_t __i = 0; __i < __n; ++__i) {                               \
      for (size_t __j = 0; __j <= __i; ++__j) {                            \
        type *__p1 = (type *)CONTAINER_OFFSET((m)->data, (m)->type_size,   \
                                              __i * __n + __j);            \
        if (__i == __j) {                                                  \
          *__p1 = conj(*__p1);                                             \
        } else {                                                           \
          type *__p2 = (type *)CONTAINER_OFFSET((m)->data, (m)->type_size, \
                                                __j * __n + __i);          \
          type __tmp = conj(*__p1);                                        \
          *__p1 = conj(*__p2);                                             \
          *__p2 = __tmp;                                                   \
        }                                                                  \
      }                                                                    \
    }                                                                      \
  })

#define LINALG_NORM(m, type)                                                  \
  ({                                                                          \
    size_t __n_eles = MATRIX_GET_NUM_ELES((m)->dim_arr, (m)->ndims);          \
    double __sum = 0;                                                         \
    for (size_t __i = 0; __i < __n_eles; ++__i) {                             \
      type __val = *(type *)CONTAINER_OFFSET((m)->data, (m)->type_size, __i); \
      __sum += creal(__val) * creal(__val) + cimag(__val) * cimag(__val);     \
    }                                                                         \
    sqrt(__sum);                                                              \
  })

#define LINALG_ANGLE(m, type)                                                 \
  ({                                                                          \
    Matrix *__angle_m = MATRIX_DUP(m);                                        \
    size_t __n_eles = MATRIX_GET_NUM_ELES((m)->dim_arr, (m)->ndims);          \
    for (size_t __i = 0; __i < __n_eles; ++__i) {                             \
      type __val = *(type *)CONTAINER_OFFSET((m)->data, (m)->type_size, __i); \
      memcpy(CONTAINER_OFFSET(__angle_m->data, (m)->type_size, __i),          \
             &(type){atan2(creal(__val), cimag(__val))}, m->type_size);       \
    }                                                                         \
    __angle_m;                                                                \
  })

#define LINALG_ANGLE_INPLACE(m, type)                                         \
  ({                                                                          \
    size_t __n_eles = MATRIX_GET_NUM_ELES((m)->dim_arr, (m)->ndims);          \
    for (size_t __i = 0; __i < __n_eles; ++__i) {                             \
      type __val = *(type *)CONTAINER_OFFSET((m)->data, (m)->type_size, __i); \
      memcpy(CONTAINER_OFFSET((m)->data, (m)->type_size, __i),                \
             &(type){atan2(creal(__val), cimag(__val))}, m->type_size);       \
    }                                                                         \
  })

#define LINALG_EXP(m, type)                                                   \
  ({                                                                          \
    Matrix *__exp_m = MATRIX_DUP(m);                                          \
    size_t __n_eles = MATRIX_GET_NUM_ELES((m)->dim_arr, (m)->ndims);          \
    for (size_t __i = 0; __i < __n_eles; ++__i) {                             \
      type __val = *(type *)CONTAINER_OFFSET((m)->data, (m)->type_size, __i); \
      memcpy(CONTAINER_OFFSET(__exp_m->data, (m)->type_size, __i),            \
             &(type){cexp(__val)}, (m)->type_size);                           \
    }                                                                         \
    __exp_m;                                                                  \
  })

#define LINALG_EXP_INPLACE(m, type)                                           \
  ({                                                                          \
    size_t __n_eles = MATRIX_GET_NUM_ELES((m)->dim_arr, (m)->ndims);          \
    for (size_t __i = 0; __i < __n_eles; ++__i) {                             \
      type __val = *(type *)CONTAINER_OFFSET((m)->data, (m)->type_size, __i); \
      memcpy(CONTAINER_OFFSET((m)->data, (m)->type_size, __i),                \
             &(type){cexp(__val)}, (m)->type_size);                           \
    }                                                                         \
  })

#define LINALG_OUTER(m1, m2, type)                                             \
  ({                                                                           \
    size_t __n1_eles = MATRIX_GET_NUM_ELES((__m1)->dim_arr, (__m1)->ndims);    \
    size_t __n2_eles = MATRIX_GET_NUM_ELES((m2)->dim_arr, (m2)->ndims);        \
    Matrix *__outer_m =                                                        \
        LINALG_ZEROS(type, (__m1)->mul_fn, (__m1)->div_fn, (__m1)->plus_fn,    \
                     (__m1)->minus_fn, (__m1)->is_zero, __n1_eles, __n2_eles); \
    for (size_t __i = 0; __i < __n1_eles; __i++) {                             \
      type __val1 =                                                            \
          *(type *)CONTAINER_OFFSET((__m1)->data, (__m1)->type_size, __i);     \
      for (size_t __j = 0; __j < n2_eles; __j++) {                             \
        type __val2 =                                                          \
            *(type *)CONTAINER_OFFSET((m2)->data, (m2)->type_size, __j);       \
        memcpy(CONTAINER_OFFSET(__outer_m->data, __outer_m->type_size,         \
                                MATRIX_GET_IDX(__outer_m, __i, __j)),          \
               &(type){__val1 * __val2}, (__m1)->type_size);                   \
      }                                                                        \
    }                                                                          \
    __outer_m;                                                                 \
  })

Matrix *linalg_eigvals(Matrix *m);

static inline double linalg_angle_cmplx(complex double x) { return carg(x); }

static inline complex double linalg_exp_cmplx(complex double x) {
  return cexp(x);
}

#endif