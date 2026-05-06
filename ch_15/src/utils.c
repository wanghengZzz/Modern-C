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
