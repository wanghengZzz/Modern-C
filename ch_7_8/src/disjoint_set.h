#ifndef __DISJOINT_SET_H_
#define __DISJOINT_SET_H_

#include <stddef.h>

typedef struct {
  size_t *parent;
  int *groups;
  size_t n;
} disjoint_set;

void *alloc_disjoint_set(size_t capacity);
size_t find(disjoint_set *s, size_t n);
int is_same_set(disjoint_set *s, size_t n1, size_t n2);
size_t disjoint_set_union(disjoint_set *s, size_t n1, size_t n2);
int free_disjoint_set(disjoint_set *ds);

#define ALLOC_DISJOINT_SET(capacity) alloc_disjoint_set(capacity)

#endif