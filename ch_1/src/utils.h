#ifndef __UTILS_H_
#define __UTILS_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

#define nullptr ((void*)0)

void swap(void *a, void *b, size_t sz);

#define CHECK_ERR(cond, err)        \
  do {                              \
    if (!(cond)) {                  \
      perror("" #err " :falied\n"); \
      exit(EXIT_FAILURE);           \
    }                               \
  } while (0);

#define CHECK_ALLOC_ERR_SAFE(ptr, err) CHECK_ERR(((void *){0} = (ptr)), err)

#define CONTAINER_OF(ptr, type, member)                   \
  __extension__({                                         \
    const typeof(((type *)0)->member) *__pmember = (ptr); \
    (type *)((char *)__pmember - offsetof(type, member)); \
  })

void *memdup(const void *, size_t);

#define ALLOC_INIT(type, n, args...) \
  (type *)memdup((type[]){args}, sizeof(type) * n)

int allocate_container(void **data, size_t type_size, size_t size);

#define REALLOCATE_CAPACITY(data, type_size, size) \
  allocate_container(&(data), type_size, size)

#define CONTAINER_OFFSET(container, type_size, offset) \
  ((void *)((char *)container + (offset) * (type_size)))

#define GET_ITEM(i, container, type, member) \
  (*((type)((container)->member) + (i)))

#define GET_DATA(i, container, type) GET_ITEM(i, container, type, data)

typedef int (*cmp_fn_t)(void const *, void const *);
typedef int (*free_fn_t)(void const *);

typedef struct container container;

struct container {
  void *data;
  size_t num_data;
  size_t type_size;
  free_fn_t free_fn;
  cmp_fn_t cmp_fn;
};

#define ALLOC_CONTAINER(type, _data, n, _cmp_fn, _free_fn) \
  ALLOC_INIT(container, 1,                                 \
             {                                             \
                 .data = memdup(_data, sizeof(type) * n),  \
                 .num_data = n,                            \
                 .type_size = sizeof(type),                \
                 .cmp_fn = _cmp_fn,                        \
                 .free_fn = _free_fn,                      \
             })

int free_container(container *c);

void *calloc_safe(size_t n, size_t type_size);
void *malloc_safe(size_t sz);

#endif