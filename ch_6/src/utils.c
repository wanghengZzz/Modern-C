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

void *meminit(void const *fill_val, size_t type_size, size_t num_data) {
  void *mem = malloc_safe(type_size * num_data);
  for (size_t i = 0; i < num_data; ++i)
    memcpy(CONTAINER_OFFSET(mem, type_size, i), fill_val, type_size);
  return mem;
}