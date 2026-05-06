#include <stdio.h>

#include "src/disjoint_set.h"
int main(void) {
  disjoint_set *s = ALLOC_DISJOINT_SET(10);
  disjoint_set_union(s, 3, 2, 2);
  disjoint_set_union(s, 1, 5, 1);
  disjoint_set_union(s, 0, 2, 0);
  printf("%d\n", is_same_set(s, 3, 0));
  printf("%d\n", is_same_set(s, 1, 5));
  printf("%d\n", is_same_set(s, 1, 2));
  printf("%d\n", is_same_set(s, 2, 5));
  free_disjoint_set(s);
  return 0;
}