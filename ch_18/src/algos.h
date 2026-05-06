#ifndef __ALGOS_H_
#define __ALGOS_H_
#include "utils.h"

void merge_sort(container *c, size_t num_thrds);
void *partition(void *args);
void merge(container *c, size_t l1, size_t r1, size_t l2, size_t r2);

void quick_sort(container *c, size_t num_thrds);
void quick_divide(container *c, int32_t l, int32_t r);
void *lomuto_quick_divide(void *args);

#endif