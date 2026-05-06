#include "utils.h"

void *memdup(void const *src, size_t sz) {
  void *mem = malloc_safe(sz);
  return memcpy(mem, src, sz);
}

void *malloc_safe(size_t sz) {
  void *mem = malloc(sz);
  CHECK_ALLOC_ERR_SAFE(mem, "malloc failed");
  return mem;
}

void *calloc_safe(size_t n, size_t sz) {
  void *mem = calloc(n, sz);
  CHECK_ALLOC_ERR_SAFE(mem, "calloc failed");
  return mem;
}