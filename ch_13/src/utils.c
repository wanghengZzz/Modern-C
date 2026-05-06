#include "utils.h"

void *memdup(const void *src, size_t sz) {
  void *mem = malloc_safe(sz);
  return memcpy(mem, src, sz);
}

void *malloc_safe(size_t sz) {
  void *mem = malloc(sz);
  CHECK_ALLOC_ERR_SAFE(mem, "malloc failed");
  return mem;
}

void *calloc_safe(size_t n, size_t type_size) {
  void *mem = calloc(n, type_size);
  CHECK_ALLOC_ERR_SAFE(mem, "calloc failed");
  return mem;
}

void *meminit(void const *fill_val, size_t type_size, size_t num_data) {
  void *mem = malloc_safe(type_size * num_data);
  for (size_t i = 0; i < num_data; ++i)
    memcpy(CONTAINER_OFFSET(mem, type_size, i), fill_val, type_size);
  return mem;
}

void swap(void *a, void *b, size_t sz) {
  char tmp[sz];
  memcpy(tmp, a, sz);
  memcpy(a, b, sz);
  memcpy(b, tmp, sz);
}