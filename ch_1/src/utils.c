#include "utils.h"

void *memdup(const void *src, size_t sz) {
  void *mem = malloc_safe(sz);
  return mem ? memcpy(mem, src, sz) : nullptr;
}

void *calloc_safe(size_t n, size_t type_size) {
  void *mem = calloc(n, type_size);
  CHECK_ALLOC_ERR_SAFE(mem, "calloc failed");
  return mem;
}

void *malloc_safe(size_t sz) {
  void *mem = malloc(sz);
  CHECK_ALLOC_ERR_SAFE(mem, "malloc failed");
  return mem;
}

int allocate_container(void **data, size_t type_size, size_t size) {
  void *mem = malloc_safe(size * type_size);
  if (*data) free(*data);
  *data = mem;
  return 1;
}

int free_container(container *c) {
  if (!c) return 0;
  if (c->data) c->free_fn(c->data);
  free(c);
  return 1;
}

void swap(void *a, void *b, size_t sz) {
  char tmp[sz];
  memcpy(tmp, a, sz);
  memcpy(a, b, sz);
  memcpy(b, tmp, sz);
}