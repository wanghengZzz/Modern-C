#ifndef __UTILS_H_
#define __UTILS_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CHECK_ERR(cond, err)        \
  do {                              \
    if (!(cond)) {                  \
      perror("" #err " :falied\n"); \
      exit(EXIT_FAILURE);           \
    }                               \
  } while (0);

#define CHECK_ALLOC_ERR_SAFE(ptr, err) CHECK_ERR(((void *){0} = (ptr)), err)

#define INIT_INC(arr, end, start) \
  for (size_t __i = start; __i < end; ++__i) (arr)[__i] = __i
#define INIT_VAL(dest, n, val, type_size) memset(dest, val, (n) * (type_size))
void swap(void *a, void *b, size_t sz);

void *malloc_safe(size_t sz);
#endif