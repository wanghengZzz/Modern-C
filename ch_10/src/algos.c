
#include "algos.h"

#include <stdio.h>
void merge(container *c, size_t l1, size_t r1, size_t l2, size_t r2) {
  size_t start_idx = l1;
  size_t tot_alloc_size = (r2 - l1 + 1) * c->type_size;
  void *data_tmp = malloc_safe(tot_alloc_size);
  void *base_tmp = data_tmp;
  while (l1 <= r1 && l2 <= r2) {
    void *data_l = CONTAINER_OFFSET(c->data, c->type_size, l1);
    void *data_r = CONTAINER_OFFSET(c->data, c->type_size, l2);
    void *src = c->cmp_fn(data_l, data_r) ? data_l : data_r;
    memcpy(data_tmp, src, c->type_size);
    data_tmp += c->type_size;
    if (src == data_l)
      l1++;
    else
      l2++;
  }
  while (l1 <= r1) {
    memcpy(data_tmp, CONTAINER_OFFSET(c->data, c->type_size, l1++),
           c->type_size);
    data_tmp += c->type_size;
  }
  while (l2 <= r2) {
    memcpy(data_tmp, CONTAINER_OFFSET(c->data, c->type_size, l2++),
           c->type_size);
    data_tmp += c->type_size;
  }
  void *dest = CONTAINER_OFFSET(c->data, c->type_size, start_idx);
  memcpy(dest, base_tmp, tot_alloc_size);
  free(base_tmp);
}

void partition(container *c, size_t l, size_t r) {
  if (l >= r) return;
  size_t m;
  m = l + ((r - l) >> 1);
  partition(c, l, m);
  partition(c, m + 1, r);
  merge(c, l, m, m + 1, r);
}

void merge_sort(container *c) { partition(c, 0, c->num_data - 1); }

void quick_divide(container *c, int32_t l, int32_t r) {
  if (l >= r) return;
  int32_t pivot_idx = l;
  for (int32_t i = l + 1; i <= r; ++i) {
    void *pivot_data = CONTAINER_OFFSET(c->data, c->type_size, pivot_idx);
    void *curr_data = CONTAINER_OFFSET(c->data, c->type_size, i);
    int32_t flag = c->cmp_fn(curr_data, pivot_data);
    if (flag) {
      if (pivot_idx + 1 == i) {
        SWAP(pivot_data, curr_data, c->type_size);
      } else {
        SWAP(curr_data, CONTAINER_OFFSET(c->data, c->type_size, pivot_idx + 1),
             c->type_size);
        SWAP(pivot_data, CONTAINER_OFFSET(c->data, c->type_size, pivot_idx + 1),
             c->type_size);
      }
      pivot_idx = i;
    }
  }
  quick_divide(c, l, pivot_idx - 1);
  quick_divide(c, pivot_idx + 1, r);
}

void lomuto_quick_divide(container *c, int32_t l, int32_t r) {
  if (l >= r) return;
  int32_t store_idx = l;
  void *pivot_data = CONTAINER_OFFSET(c->data, c->type_size, r);
  void *target;

  for (int32_t i = l; i < r; ++i) {
    void *curr_data = CONTAINER_OFFSET(c->data, c->type_size, i);

    if (c->cmp_fn(curr_data, pivot_data)) {
      target = CONTAINER_OFFSET(c->data, c->type_size, store_idx);
      if (curr_data != target)
        swap(curr_data, target,
            c->type_size);
      store_idx++;
    }
  }
  target = CONTAINER_OFFSET(c->data, c->type_size, store_idx);
  if (target != pivot_data) {
    swap(target, pivot_data,
       c->type_size);
  }
  lomuto_quick_divide(c, l, store_idx - 1);
  lomuto_quick_divide(c, store_idx + 1, r);
}

void quick_sort(container *c) {
  // quick_divide(c, 0, c->num_data - 1);
  lomuto_quick_divide(c, 0, c->num_data - 1);
}