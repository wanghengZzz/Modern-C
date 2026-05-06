#ifndef __UTILS_H_
#define __UTILS_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const size_t __bits_32[32];

#define BITS_32(n) __bits_32[n]

#define CONTAINER_OF(ptr, type, member)                   \
  __extension__({                                         \
    const typeof(((type *)0)->member) *__pmember = (ptr); \
    (type *)((char *)__pmember - offsetof(type, member)); \
  })

int allocate_container(void **, size_t, size_t);
#define REALLOCATE_CAPACITY(data, type_size, size) \
  allocate_container(&(data), type_size, size)

#define CONTAINER_OFFSET(container, type_size, offset) \
  ((void *)((char *)container + (offset) * (type_size)))

#define INIT_INC(arr, end, start) \
  for (size_t __i = start; __i < end; ++__i) (arr)[__i] = __i
#define INIT_VAL(dest, n, val, type_size) memset(dest, val, (n) * (type_size))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define IS_POW2(n) ((n) && !((n) & ((n) - 1)))

void *memdup(const void *src, size_t sz);
void *malloc_safe(size_t sz);
void swap(void *a, void *b, size_t sz);

#define CHECK_ERR(cond, err)        \
  do {                              \
    if (!(cond)) {                  \
      perror("" #err " :falied\n"); \
      exit(EXIT_FAILURE);           \
    }                               \
  } while (0);

#define CHECK_ALLOC_ERR_SAFE(ptr, err) CHECK_ERR(((void *){0} = (ptr)), err)

#define ALLOC_INIT(type, n, args...) \
  (type *)memdup((type[]){args}, sizeof(type) * n)


#endif