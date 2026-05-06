#ifndef __ALGOS_H_
#define __ALGOS_H_
#include "utils.h"

void merge_sort(container *c);
void partition(container *c, size_t l, size_t r);
void merge(container *c, size_t l1, size_t r1, size_t l2, size_t r2);

void quick_sort(container *c);
void quick_divide(container *c, int32_t l, int32_t r);
void lomuto_quick_divide(container *c, int32_t l, int32_t r);

#endif