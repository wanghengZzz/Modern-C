
#include <stdio.h>
#include <time.h>

#include "src/algos.h"
#include "src/io.h"
#include "src/utils.h"

int cmp_string_inc(void const *s1, void const *s2) {
  return strcmp((*(CharArray *)s1).str, (*(CharArray *)s2).str);
}

int cmp_string_dec(void const *s1, void const *s2) {
  return strcmp((*(CharArray *)s2).str, (*(CharArray *)s1).str);
}


int cmp_dbl_inc(void const *s1, void const *s2) {
  return (*(double *)s1 - *(double *)s2) < 0 ? -1 : 1;
}

int cmp_dbl_dec(void const *s1, void const *s2) {
  return (*(double *)s2 - *(double *)s1) < 0 ? -1 : 1;
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

void print_string_ret(container *c, size_t prt_n) {
  for (size_t i = 0; i < prt_n; ++i) {
    printf("%zu: %s\n", i, GET_DATA(i, c, CharArray *).str);
  }
}

void print_dbl_ret(container *c, size_t prt_n) {
  for (size_t i = 0; i < prt_n; ++i) {
    printf("%zu: %.10f\n", i, GET_DATA(i, c, double *));
  }
}
int clear_and_recover_data(void **dest, void const *src, size_t data_size) {
  if (!dest || !src || !data_size) return 0;
  *dest = memdup(src, data_size);
  return *dest ? 1 : 0;
}

void eval(sort_func_t s_fn, container *c, char const *method_name) {
  printf("\nEval %s :\n", method_name);
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  s_fn(c);
  clock_gettime(CLOCK_MONOTONIC, &end);
  double time_consume = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("Time: %.6f secs\n\n", time_consume);
}

void eval_qsort(container *c, char const *method_name) {
  printf("\nEval %s :\n", method_name);
  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);
  qsort(c->data, c->num_data, c->type_size, c->cmp_fn);
  clock_gettime(CLOCK_MONOTONIC, &end);
  double time_consume = (end.tv_sec - start.tv_sec) +
                        (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("Time: %.6f secs\n\n", time_consume);
}

int main(int argc, char *argv[argc + 1]) {
  container *c = NULL;
  size_t max_n_str = 22000;
  char const *const str_arr[] = {
      "zeg", "eric", "juice", "apple", "pineapple", "eabcas",
  };
  CharArray *strings;

  if (argc > 1) {
    strings = calloc_safe(max_n_str, sizeof(CharArray));
    max_n_str = read_file(argv[1], strings, max_n_str);
  } else {
    strings = calloc_safe(ARRAY_SIZE(str_arr), sizeof(CharArray));
    for (size_t i = 0; i < ARRAY_SIZE(str_arr); ++i)
      memcpy(strings[i].str, str_arr[i], strlen(str_arr[i]));
  }

  double const dbl_arr[] = {
      1e-7, 3.14, 4e-3, 5e-9, 1.2, 3.3, 3e-2,
  };

  c = ALLOC_CONTAINER(CharArray, strings, max_n_str, cmp_string_dec,
                      free_fn_string);

  const size_t prt_max_n = 30;
  printf("\nMerge Sort for strings:\n");
  eval(merge_sort, c, "Merge Sort");
  print_string_ret(c, MIN(prt_max_n, c->num_data));
  free(c->data);
  clear_and_recover_data(&c->data, strings, sizeof(CharArray) * max_n_str);

  printf("\n Qsort in C Library for strings:\n");
  eval_qsort(c, "Qsort");
  print_string_ret(c, MIN(prt_max_n, c->num_data));
  free(c->data);
  clear_and_recover_data(&c->data, strings, sizeof(CharArray) * max_n_str);

  printf("\nQuick Sort for strings:\n");
  eval(quick_sort, c, "Quick Sort");
  print_string_ret(c, MIN(prt_max_n, c->num_data));
  free_container(c);

  c = ALLOC_CONTAINER(double, dbl_arr, ARRAY_SIZE(dbl_arr), cmp_dbl_dec,
                      free_fn_dbl);

  printf("\nMerge Sort for double:\n");
  eval(merge_sort, c, "Merge Sort");
  print_dbl_ret(c, MIN(prt_max_n, c->num_data));
  free(c->data);
  clear_and_recover_data(&c->data, dbl_arr, sizeof(dbl_arr));

  printf("\n Qsort in C Library for strings:\n");
  eval_qsort(c, "Qsort");
  print_dbl_ret(c, MIN(prt_max_n, c->num_data));
  free(c->data);
  clear_and_recover_data(&c->data, dbl_arr, sizeof(dbl_arr));

  printf("\nQuick Sort for double:\n");
  eval(quick_sort, c, "Quick Sort");
  print_dbl_ret(c, MIN(prt_max_n, c->num_data));
  free_container(c);

  free(strings);
  return 0;
}