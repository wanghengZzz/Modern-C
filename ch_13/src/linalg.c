#include "linalg.h"

Matrix *linalg_householder_hessenberg(Matrix *m) {
  size_t n = m->dim_arr[0];
  Matrix *m_dup = MATRIX_DUP(m);

  for (size_t k = 0; k < n - 2; ++k) {
    size_t len = n - k - 1;
    Matrix *x = LINALG_ZEROS(complex double, m->mul_fn, m->div_fn, m->plus_fn,
                             m->minus_fn, m->is_zero, len);

    for (size_t i = 0; i < len; ++i) {
      complex double val = *(complex double *)CONTAINER_OFFSET(
          m_dup->data, m_dup->type_size, MATRIX_GET_IDX(m_dup, k + 1 + i, k));
      memcpy(CONTAINER_OFFSET(x->data, x->type_size, i), &val, x->type_size);
    }

    double norm_x = LINALG_NORM(x, complex double);

    Matrix *v = MATRIX_DUP(x);
    complex double x_0 = *(complex double *)(x->data);
    complex double shift =
        linalg_exp_cmplx(I * linalg_angle_cmplx(x_0)) * norm_x;
    *(complex double *)(v->data) += shift;
    norm_x = LINALG_NORM(v, complex double);
    MATRIX_DIV_SCALAR_INPLACE(v, complex double, norm_x);

    size_t cols_left = n - k;
    complex double *w_left = calloc_safe(cols_left, sizeof(complex double));

    for (size_t j = 0; j < cols_left; ++j) {
      for (size_t i = 0; i < len; ++i) {
        complex double vi =
            *(complex double *)CONTAINER_OFFSET(v->data, v->type_size, i);
        complex double hij = *(complex double *)CONTAINER_OFFSET(
            m_dup->data, m_dup->type_size,
            MATRIX_GET_IDX(m_dup, k + 1 + i, k + j));
        w_left[j] += conj(vi) * hij;
      }
    }

    for (size_t i = 0; i < len; ++i) {
      complex double vi =
          *(complex double *)CONTAINER_OFFSET(v->data, v->type_size, i);
      for (size_t j = 0; j < cols_left; ++j) {
        complex double *pos = (complex double *)CONTAINER_OFFSET(
            m_dup->data, m_dup->type_size,
            MATRIX_GET_IDX(m_dup, k + 1 + i, k + j));
        *pos -= 2.0 * vi * w_left[j];
      }
    }
    free(w_left);

    complex double *w_right = calloc_safe(n, sizeof(complex double));

    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < len; ++j) {
        complex double hij = *(complex double *)CONTAINER_OFFSET(
            m_dup->data, m_dup->type_size, MATRIX_GET_IDX(m_dup, i, k + 1 + j));
        complex double vj =
            *(complex double *)CONTAINER_OFFSET(v->data, v->type_size, j);
        w_right[i] += hij * vj;
      }
    }
    // m_dup[:, k+1:] -= 2 * outer(w_right, v^H)
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < len; ++j) {
        complex double vj =
            *(complex double *)CONTAINER_OFFSET(v->data, v->type_size, j);
        complex double *pos = (complex double *)CONTAINER_OFFSET(
            m_dup->data, m_dup->type_size, MATRIX_GET_IDX(m_dup, i, k + 1 + j));
        *pos -= 2.0 * w_right[i] * conj(vj);
      }
    }
    free(w_right);

    free_matrix(x);
    free_matrix(v);
  }
  return m_dup;
}

void linalg_qr(Matrix *m, Matrix **Q_out, Matrix **R_out) {
  size_t n = m->dim_arr[0];

  Matrix *R = MATRIX_DUP(m);

  Matrix *Q = LINALG_IDENTITY(n, complex double, m->mul_fn, m->div_fn,
                              m->plus_fn, m->minus_fn, m->is_zero);

  Matrix *tmp = MATRIX_DUP(m);

  for (size_t i = 1; i < n; ++i) {
    size_t j = i - 1;
    complex double val = *(complex double *)CONTAINER_OFFSET(
        R->data, R->type_size, MATRIX_GET_IDX(R, i, j));
    if (cabs(val) < 1e-14) continue;

    Matrix *G = LINALG_Q(R, complex double, n, i, j, m->mul_fn, m->div_fn,
                         m->plus_fn, m->minus_fn, m->is_zero);

    matrix_mul_dbl_cmplx(G, R, tmp);
    memcpy(R->data, tmp->data,
           tmp->type_size * MATRIX_GET_NUM_ELES(tmp->dim_arr, tmp->ndims));

    LINALG_CONJ_T_INPLACE(G, complex double);

    matrix_mul_dbl_cmplx(Q, G, tmp);
    memcpy(Q->data, tmp->data,
           tmp->type_size * MATRIX_GET_NUM_ELES(tmp->dim_arr, tmp->ndims));

    free_matrix(G);
  }

  free_matrix(tmp);
  *Q_out = Q;
  *R_out = R;
}

Matrix *linalg_eigvals(Matrix *m) {
  size_t n = m->dim_arr[0];

  // Hessenberg => QR O(n²)
  Matrix *H = linalg_householder_hessenberg(m);

  Matrix *tmp = LINALG_ZEROS(complex double, m->mul_fn, m->div_fn, m->plus_fn,
                             m->minus_fn, m->is_zero, n, n);

  {
    Matrix *Hw = MATRIX_DUP(H);
    size_t sz = n;

    while (sz > 1) {
      int deflated = 0;

      for (size_t iter = 0; iter < 3000 && !deflated; ++iter) {
        // --- Wilkinson 2×2 shift on bottom-right of sz×sz block ---
        complex double sigma;
        if (iter > 0 && iter % 10 == 0) {
          /*
            if eigenvalues of 2 x 2 matrix are zeros
            like A = [[0, 0], [1, 0]]
            A - 0*I = A and A is orthogonal matrix
            => QR decomp => Q = A, R = I
            RQ + 0*I = I x A = A
            => A is still the same
            => adding an exceptional shift (a complex number)
          */
          complex double c_w = *(complex double *)CONTAINER_OFFSET(
              Hw->data, Hw->type_size, MATRIX_GET_IDX(Hw, sz - 1, sz - 2));
          complex double b_w = *(complex double *)CONTAINER_OFFSET(
              Hw->data, Hw->type_size, MATRIX_GET_IDX(Hw, sz - 2, sz - 1));
          sigma = (cabs(c_w) + cabs(b_w)) * 0.75 + (complex double)(0.1 * I);
        } else {
          complex double a_w = *(complex double *)CONTAINER_OFFSET(
              Hw->data, Hw->type_size, MATRIX_GET_IDX(Hw, sz - 2, sz - 2));
          complex double b_w = *(complex double *)CONTAINER_OFFSET(
              Hw->data, Hw->type_size, MATRIX_GET_IDX(Hw, sz - 2, sz - 1));
          complex double c_w = *(complex double *)CONTAINER_OFFSET(
              Hw->data, Hw->type_size, MATRIX_GET_IDX(Hw, sz - 1, sz - 2));
          complex double d_w = *(complex double *)CONTAINER_OFFSET(
              Hw->data, Hw->type_size, MATRIX_GET_IDX(Hw, sz - 1, sz - 1));
          complex double tr_w = a_w + d_w;
          complex double det_w = a_w * d_w - b_w * c_w;
          complex double disc_w = csqrt(tr_w * tr_w - 4.0 * det_w);
          complex double e1_w = (tr_w + disc_w) / 2.0;
          complex double e2_w = (tr_w - disc_w) / 2.0;
          sigma = (cabs(e1_w - d_w) < cabs(e2_w - d_w)) ? e1_w : e2_w;
        }

        Matrix *sub =
            LINALG_ZEROS(complex double, Hw->mul_fn, Hw->div_fn, Hw->plus_fn,
                         Hw->minus_fn, Hw->is_zero, sz, sz);
        for (size_t r = 0; r < sz; ++r)
          for (size_t c = 0; c < sz; ++c) {
            complex double v = *(complex double *)CONTAINER_OFFSET(
                Hw->data, Hw->type_size, MATRIX_GET_IDX(Hw, r, c));
            if (r == c) v -= sigma;
            memcpy(CONTAINER_OFFSET(sub->data, sub->type_size,
                                    MATRIX_GET_IDX(sub, r, c)),
                   &v, sub->type_size);
          }

        Matrix *Qsub = NULL, *Rsub = NULL;
        linalg_qr(sub, &Qsub, &Rsub);

        Matrix *RQ =
            LINALG_ZEROS(complex double, Hw->mul_fn, Hw->div_fn, Hw->plus_fn,
                         Hw->minus_fn, Hw->is_zero, sz, sz);
        matrix_mul_dbl_cmplx(Rsub, Qsub, RQ);

        // write back to Hw[:sz,:sz] = RQ + sigma*I
        for (size_t r = 0; r < sz; ++r)
          for (size_t c = 0; c < sz; ++c) {
            complex double v = *(complex double *)CONTAINER_OFFSET(
                RQ->data, RQ->type_size, MATRIX_GET_IDX(RQ, r, c));
            if (r == c) v += sigma;
            memcpy(CONTAINER_OFFSET(Hw->data, Hw->type_size,
                                    MATRIX_GET_IDX(Hw, r, c)),
                   &v, Hw->type_size);
          }

        free_matrix(sub);
        free_matrix(Qsub);
        free_matrix(Rsub);
        free_matrix(RQ);

        // Check Hw[sz-1, sz-2] is small enough → deflate
        complex double sub_diag = *(complex double *)CONTAINER_OFFSET(
            Hw->data, Hw->type_size, MATRIX_GET_IDX(Hw, sz - 1, sz - 2));
        if (cabs(sub_diag) < 1e-10) {
          memcpy(CONTAINER_OFFSET(Hw->data, Hw->type_size,
                                  MATRIX_GET_IDX(Hw, sz - 1, sz - 2)),
                 &(complex double){0.0}, Hw->type_size);
          sz--;
          deflated = 1;
        }
      }
      if (!deflated) sz--;
    }

    memcpy(H->data, Hw->data,
           Hw->type_size * MATRIX_GET_NUM_ELES(Hw->dim_arr, Hw->ndims));
    free_matrix(Hw);
  }

  Matrix *eigvals = LINALG_ZEROS(complex double, m->mul_fn, m->div_fn,
                                 m->plus_fn, m->minus_fn, m->is_zero, n);
  for (size_t i = 0; i < n; ++i) {
    complex double v = *(complex double *)CONTAINER_OFFSET(
        H->data, H->type_size, MATRIX_GET_IDX(H, i, i));
    memcpy(CONTAINER_OFFSET(eigvals->data, eigvals->type_size, i), &v,
           eigvals->type_size);
#ifdef DEBUG
    printf("eigenvalue[%zu] = %.6f + %.6fi\n", i, creal(v), cimag(v));
#endif
  }

  free_matrix(H);
  free_matrix(tmp);
  return eigvals;
}