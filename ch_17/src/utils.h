#ifndef __UTILS_H_
#define __UTILS_H_

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define nullptr ((void *)0)
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void *memdup(void const *src, size_t sz);

#define ALLOC_INIT(type, n, args...) \
  (type *)memdup((type[]){args}, sizeof(type) * n)

#define CONTAINER_OFFSET(container, type_size, offset) \
  ((void *)((char *)container + (offset) * (type_size)))

#define CHECK_ERR(cond, err)        \
  do {                              \
    if (!(cond)) {                  \
      perror("" #err " :falied\n"); \
      exit(EXIT_FAILURE);           \
    }                               \
  } while (0);

#define CHECK_ALLOC_ERR_SAFE(ptr, err) CHECK_ERR(((void *){0} = (ptr)), err)

void *malloc_safe(size_t sz);
int allocate_container(void **, size_t, size_t);

#define REALLOCATE_CAPACITY(data, type_size, size) \
  allocate_container(&(data), type_size, size)

#endif