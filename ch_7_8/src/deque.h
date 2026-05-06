#ifndef __DEQUE_H_
#define __DEQUE_H_

#include <stddef.h>

typedef struct {
  void *container;
  size_t size;
  size_t capacity;
  size_t type_size;
  size_t front, back;
} deque;

int free_deque(deque *dq);
void *alloc_deque(size_t size, size_t type_size);
int realloc_deque(deque *dq, size_t size, size_t type_size);
int deque_push_back(deque *dq, void *data);
int deque_pop_back(deque *dq);
int deque_pop_front(deque *dq);
int deque_push_front(deque *dq, void *data);

#define DEQUE_PUSH_BACK(dq, val) deque_push_back(dq, ((void *){0} = &(val)))

#define DEQUE_POP_BACK(dq) deque_pop_back(dq)

#define DEQUE_PUSH_FRONT(dq, val) deque_push_front(dq, ((void *){0} = &(val)))

#define DEQUE_POP_FRONT(dq) deque_pop_front(dq)

#define DEQUE_SIZE(dq) ((dq)->size)
#define DEQUE_CAPACITY(dq) ((dq)->capacity)
#define DEQUE_BACK(dq, type)            \
  (*(type *)CONTAINER_OFFSET(           \
      (dq)->container, (dq)->type_size, \
      ((dq)->back - 1 + (dq)->capacity) % (dq)->capacity))

#define DEQUE_FRONT(dq, type) \
  (*(type *)CONTAINER_OFFSET((dq)->container, (dq)->type_size, (dq)->front))
#define DEQUE_IS_EMPTY(dq) ((dq)->size == 0)
#define DEQUE_IS_FULL(dq) ((dq)->size == (dq)->capacity - 1)

#define ALLOC_DEQUE(size, type_size) alloc_deque(size, type_size)

#define FREE_DEQUE(dq) free_deque(dq)

#endif