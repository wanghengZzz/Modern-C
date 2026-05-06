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

size_t find_replace(disjoint_set *s, size_t n, size_t specific_val) {
  return n == s->parent[n]
             ? (s->parent[n] = specific_val)
             : (s->parent[n] = find_replace(s, s->parent[n], specific_val));
}

size_t find_compress(disjoint_set *s, size_t n) {
  return n == s->parent[n] ? n
                           : (s->parent[n] = find_compress(s, s->parent[n]));
}

int is_same_set(disjoint_set *s, size_t n1, size_t n2) {
  return find(s, n1) == find(s, n2);
}

size_t disjoint_set_union(disjoint_set *s, size_t n1, size_t n2,
                          size_t specific_val) {
  size_t p1 = find_replace(s, n1, specific_val);
  size_t p2 = find_compress(s, n2);
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