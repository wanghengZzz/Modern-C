
#include "algos.h"

#include <pthread.h>
#include <stdio.h>

void merge(container *c, size_t l1, size_t r1, size_t l2, size_t r2) {
  size_t start_idx = l1;
  size_t tot_alloc_size = (r2 - l1 + 1) * c->type_size;
  char buf[tot_alloc_size];
  void *data_tmp = buf;
  void *base_tmp = data_tmp;
  while (l1 <= r1 && l2 <= r2) {
    void *data_l = CONTAINER_OFFSET(c->data, c->type_size, l1);
    void *data_r = CONTAINER_OFFSET(c->data, c->type_size, l2);
    void *src = c->cmp_fn(data_l, data_r) < 0 ? data_l : data_r;
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
}

void *partition(void *args) {
#ifdef DEBUG
  printf("Thread ID: %lu\n", (unsigned long)pthread_self());
#endif
  size_t l = ((sort_args_t *)args)->l;
  size_t r = ((sort_args_t *)args)->r;
  if (l >= r) return nullptr;

  container *c = ((sort_args_t *)args)->c;
  long remain_thrds = ((sort_args_t *)args)->thrds - 2;
  size_t m;
  m = l + ((r - l) >> 1);
  sort_args_t *pl_args =
                  &(sort_args_t){
                      .c = c,
                      .l = l,
                      .r = m,
                      .thrds = remain_thrds >> 1,
                  },
              *pr_args = &(sort_args_t){
                  .c = c,
                  .l = m + 1,
                  .r = r,
                  .thrds = remain_thrds >> 1,
              };

  if (remain_thrds >= 0) {
    pthread_t t_pl, t_pr;
    pthread_create(&t_pl, nullptr, partition, pl_args);
    pthread_create(&t_pr, nullptr, partition, pr_args);
    pthread_join(t_pl, nullptr);
    pthread_join(t_pr, nullptr);
  } else {
    partition(pl_args);
    partition(pr_args);
  }
  merge(c, l, m, m + 1, r);
  return nullptr;
}

void merge_sort(container *c, size_t num_thrds) {
  partition(
      &(sort_args_t){.c = c, .l = 0, .r = c->num_data - 1, .thrds = num_thrds});
}

void quick_divide(container *c, int32_t l, int32_t r) {
  if (l >= r) return;
  int32_t pivot_idx = l;
  for (int32_t i = l + 1; i <= r; ++i) {
    void *pivot_data = CONTAINER_OFFSET(c->data, c->type_size, pivot_idx);
    void *curr_data = CONTAINER_OFFSET(c->data, c->type_size, i);
    int32_t flag = c->cmp_fn(curr_data, pivot_data) < 0;
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

void *lomuto_quick_divide(void *args) {
  size_t l = ((sort_args_t *)args)->l;
  size_t r = ((sort_args_t *)args)->r;
  container *c = ((sort_args_t *)args)->c;
  long remain_thrds = ((sort_args_t *)args)->thrds / 2 - 1;
  long signed_l = l, signed_r = r;
  if (signed_l >= signed_r) return nullptr;

  long store_idx = signed_l;
  void *pivot_data = CONTAINER_OFFSET(c->data, c->type_size, signed_r);
  void *target;
  for (long i = signed_l; i < signed_r; ++i) {
    void *curr_data = CONTAINER_OFFSET(c->data, c->type_size, i);

    if (c->cmp_fn(curr_data, pivot_data) < 0) {
      target = CONTAINER_OFFSET(c->data, c->type_size, store_idx);
      if (curr_data != target)
        SWAP(curr_data, CONTAINER_OFFSET(c->data, c->type_size, store_idx),
             c->type_size);
      store_idx++;
    }
  }
  target = CONTAINER_OFFSET(c->data, c->type_size, store_idx);
  if (target != pivot_data) {
    swap(target, pivot_data, c->type_size);
  }
  sort_args_t *pl_args =
                  &(sort_args_t){
                      .c = c,
                      .l = l,
                      .r = store_idx - 1,
                      .thrds = remain_thrds,
                  },
              *pr_args = &(sort_args_t){
                  .c = c,
                  .l = store_idx + 1,
                  .r = r,
                  .thrds = remain_thrds,
              };

  if (remain_thrds >= 0) {
    pthread_t t_pl, t_pr;
    if (store_idx > signed_l)
      pthread_create(&t_pl, nullptr, lomuto_quick_divide, pl_args);
    pthread_create(&t_pr, nullptr, lomuto_quick_divide, pr_args);
    if (store_idx > signed_l) pthread_join(t_pl, nullptr);
    pthread_join(t_pr, nullptr);
  } else {
    if (store_idx > signed_l) lomuto_quick_divide(pl_args);
    lomuto_quick_divide(pr_args);
  }
  return nullptr;
}

void quick_sort(container *c, size_t num_thrds) {
  // quick_divide(c, 0, c->num_data - 1);
  // lomuto_quick_divide(c, 0, c->num_data - 1);
  lomuto_quick_divide(
      &(sort_args_t){.c = c, .l = 0, .r = c->num_data - 1, .thrds = num_thrds});
}