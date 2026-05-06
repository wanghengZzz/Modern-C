#include "disjoint_set.h"

#include <stdlib.h>

#include "utils.h"

void *alloc_disjoint_set(size_t capacity) {
  disjoint_set *ds = malloc_safe(sizeof(disjoint_set));
  if (!ds) return NULL;

  ds->groups = malloc_safe(sizeof(int) * capacity);
  ds->parent = malloc_safe(sizeof(size_t) * capacity);
  ds->n = capacity;
  if (!ds->groups || !ds->parent) return NULL;

  INIT_VAL(ds->groups, capacity, 1, sizeof(int));
  INIT_INC(ds->parent, capacity, 0);

  return ds;
}

size_t find(disjoint_set *s, size_t n) {
  return n == s->parent[n] ? n : (s->parent[n] = find(s, s->parent[n]));
}

int is_same_set(disjoint_set *s, size_t n1, size_t n2) {
  return find(s, n1) == find(s, n2);
}

size_t disjoint_set_union(disjoint_set *s, size_t n1, size_t n2) {
  size_t p1 = find(s, n1);
  size_t p2 = find(s, n2);
  if (p1 == p2) return p1;
  if (s->groups[p1] > s->groups[p2]) {
    s->parent[p2] = p1;
    s->groups[p1] += s->groups[p2];
    s->groups[p2] = 0;
    return p1;
  } else {
    s->parent[p1] = p2;
    s->groups[p2] += s->groups[p1];
    s->groups[p1] = 0;
    return p2;
  }
}

int free_disjoint_set(disjoint_set *ds) {
  if (!ds) return 0;
  if (ds->groups) {
    free(ds->groups);
    ds->groups = NULL;
  }
  if (ds->parent) {
    free(ds->parent);
    ds->parent = NULL;
  }
  free(ds);
  ds = NULL;
  return 1;
}

#ifdef DEBUG_DISJOINT_SET
#include <stdio.h>
int main(void) {
  disjoint_set *s = ALLOC_DISJOINT_SET(10);
  disjoint_set_union(s, 3, 2);
  disjoint_set_union(s, 1, 5);
  disjoint_set_union(s, 0, 2);
  printf("%d\n", is_same_set(s, 3, 0));
  printf("%d\n", is_same_set(s, 1, 5));
  return 0;
}
#endif