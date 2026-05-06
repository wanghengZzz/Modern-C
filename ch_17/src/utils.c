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

void *realloc_safe(void *ptr, size_t sz) {
  void *mem = realloc(ptr, sz);
  CHECK_ALLOC_ERR_SAFE(mem, "realloc failed");
  return mem;
}

int allocate_container(void **data, size_t type_size, size_t size) {
  void *mem = realloc_safe(*data, type_size * size);
  *data = mem;
  return 1;
}
