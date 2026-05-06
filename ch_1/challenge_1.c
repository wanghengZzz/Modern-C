#include <stdio.h>
#include <string.h>

#include "src/algos.h"
#include "src/utils.h"
int cmp_fn_string(void const *s1, void const *s2) {
  return strcmp(*(char const **)s1, *(char const **)s2);
}

int cmp_fn_dbl(void const *s1, void const *s2) {
  return (*(double *)s1 - *(double *)s2) < 0 ? -1 : 1;
}

int free_fn_string(void const *s1) {
  if (!s1) return 0;
  free((void *)s1);
  return 1;
}

int free_fn_dbl(void const *s1) {
  if (!s1) return 0;
  free((void *)s1);
  return 1;
}

void print_string_ret(container *c) {
  for (size_t i = 0; i < c->num_data; ++i) {
    printf("%zu: %s\n", i, GET_DATA(i, c, char **));
  }
}

void print_dbl_ret(container *c) {
  for (size_t i = 0; i < c->num_data; ++i) {
    printf("%zu: %.10f\n", i, GET_DATA(i, c, double *));
  }
}

int clear_and_recover_data(void **dest, void const *src, size_t data_size) {
  if (!dest || !src || !data_size) return 0;
  *dest = memdup(src, data_size);
  return *dest ? 1 : 0;
}

int main(void) {
  container *c = NULL;
  char const *const strings[] = {
      "zeg", "eric", "juice", "apple", "pineapple", "eabcas",
  };

  double const dbl_arr[] = {
      1e-7, 3.14, 4e-3, 5e-9, 1.2, 3.3, 3e-2,
  };
  c = ALLOC_CONTAINER(char *, strings, ARRAY_SIZE(strings), cmp_fn_string,
                      free_fn_string);

  printf("\nMerge Sort for strings:\n");
  merge_sort(c);
  print_string_ret(c);
  free(c->data);
  clear_and_recover_data(&c->data, strings, sizeof(strings));
  printf("\nQuick Sort for strings:\n");
  quick_sort(c);
  print_string_ret(c);
  free_container(c);

  c = ALLOC_CONTAINER(double, dbl_arr, ARRAY_SIZE(dbl_arr), cmp_fn_dbl,
                      free_fn_dbl);

  printf("\nMerge Sort for double:\n");
  merge_sort(c);
  print_dbl_ret(c);
  free(c->data);
  clear_and_recover_data(&c->data, dbl_arr, sizeof(dbl_arr));

  printf("\nQuick Sort for double:\n");
  quick_sort(c);
  print_dbl_ret(c);
  free_container(c);

  return 0;
}