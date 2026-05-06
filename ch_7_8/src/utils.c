#include "utils.h"

const size_t __bits_32[32] = {
    1u << 31, 1u << 30, 1u << 29, 1u << 28, 1u << 27, 1u << 26, 1u << 25,
    1u << 24, 1u << 23, 1u << 22, 1u << 21, 1u << 20, 1u << 19, 1u << 18,
    1u << 17, 1u << 16, 1u << 15, 1u << 14, 1u << 13, 1u << 12, 1u << 11,
    1u << 10, 1u << 9,  1u << 8,  1u << 7,  1u << 6,  1u << 5,  1u << 4,
    1u << 3,  1u << 2,  1u << 1,  1u << 0};

void *memdup(const void *src, size_t sz) {
  void *mem = malloc_safe(sz);
  return memcpy(mem, src, sz);
}

void *malloc_safe(size_t sz) {
  void *mem = malloc(sz);
  CHECK_ALLOC_ERR_SAFE(mem, "malloc failed");
  return mem;
}

void swap(void *a, void *b, size_t sz) {
  char tmp[sz];
  memcpy(tmp, a, sz);
  memcpy(a, b, sz);
  memcpy(b, tmp, sz);
}

int allocate_container(void **data, size_t type_size, size_t size) {
  void *mem = malloc_safe(size * type_size);
  if (*data) free(*data);
  *data = mem;
  return 1;
}