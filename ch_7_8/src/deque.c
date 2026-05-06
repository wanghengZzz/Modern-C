#include "deque.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

int free_deque(deque *dq) {
  if (!dq) return 0;
  if (dq->container) {
    free(dq->container);
    dq->container = NULL;
  }
  free(dq);
  dq = NULL;
  return 1;
}

void *alloc_deque(size_t size, size_t type_size) {
  deque *dq = malloc_safe(sizeof(deque));
  dq->container = NULL;
  if (!dq || !REALLOCATE_CAPACITY(dq->container, type_size, size + 1))
    return NULL;
  dq->size = 0;
  dq->capacity = size + 1;
  dq->back = 0;
  dq->type_size = type_size;
  dq->front = 0;
  return dq;
}

int realloc_deque(deque *dq, size_t size, size_t type_size) {
  if (!dq || !dq->container || size <= dq->capacity) return 0;

  void *new_container = realloc(dq->container, size * type_size);
  if (!new_container) return 0;
  dq->container = new_container;
  if (dq->front > dq->back) {
    size_t n_data = dq->capacity - dq->front;
    size_t new_front = size - n_data;
    memmove(CONTAINER_OFFSET(dq->container, dq->type_size, new_front),
            CONTAINER_OFFSET(dq->container, dq->type_size, dq->front),
            dq->type_size * n_data);
    dq->front = new_front;
  }
  dq->capacity = size;
  return 1;
}

int deque_push_back(deque *dq, void *data) {
  if ((dq->back + 1) % dq->capacity == dq->front) return 0;

  memcpy(CONTAINER_OFFSET(dq->container, dq->type_size, dq->back), data,
         dq->type_size);

  dq->back = (dq->back + 1) % dq->capacity;
  dq->size++;
  return 1;
}

int deque_pop_back(deque *dq) {
  if (dq->front == dq->back) return 0;

  dq->back = (dq->back + dq->capacity - 1) % dq->capacity;
  dq->size--;
  return 1;
}

int deque_pop_front(deque *dq) {
  if (dq->back == dq->front) return 0;

  dq->front = (dq->front + 1) % dq->capacity;
  dq->size--;
  return 1;
}

int deque_push_front(deque *dq, void *data) {
  size_t new_front = (dq->front + dq->capacity - 1) % dq->capacity;
  if (new_front == dq->back) return 0;
  memcpy(CONTAINER_OFFSET(dq->container, dq->type_size, new_front), data,
         dq->type_size);
  dq->front = new_front;
  dq->size++;
  return 1;
}
