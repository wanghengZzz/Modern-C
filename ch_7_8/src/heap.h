#ifndef __HEAP_H_
#define __HEAP_H_

#include "utils.h"

typedef struct tree_node {
  void *data;
  size_t data_sz;
  struct tree_node *left, *right, *parent;
} tree_node;

typedef int (*cmp_fn_t)(void const *, void const *);

typedef struct {
  tree_node *root;
  tree_node *last_parent;
  size_t num_nodes;
  size_t deleted_num_nodes;
  size_t allocated_nodes;
  cmp_fn_t cmp_fn;
  void *fill_val;
} heap;

tree_node *find_leftest_node(tree_node *root);
tree_node *find_next_parent(tree_node *parent, tree_node *root, size_t idx,
                            size_t pos);
int heap_insert(heap *hp, tree_node *node);
int heap_pop(heap *hp);
void *heap_get_top(heap *hp);
int heap_bottom_up_update(heap *hp, tree_node *node);
int heap_top_down_update(heap *hp, tree_node *node);
int heap_is_empty(heap *hp);
void free_tree_nodes(tree_node *node);
int free_heap(heap *hp);

#define HEAP_IS_EMPTY(hp) heap_is_empty(hp)

#define NEW_NODE(type, _from, _to, _w)                                  \
  &((tree_node){                                                        \
      .data = ALLOC_INIT(type, 1, {.from = _from, .to = _to, .w = _w}), \
      .data_sz = sizeof(type),                                          \
      .left = NULL,                                                     \
      .right = NULL,                                                    \
      .parent = NULL,                                                   \
  })

#define ALLOC_HEAP(type, _cmp_fn, _fill_val_fn)                   \
  ALLOC_INIT(heap, 1,                                             \
             {                                                    \
                 .root = NULL,                                    \
                 .last_parent = NULL,                             \
                 .num_nodes = 0,                                  \
                 .allocated_nodes = 0,                     \
                 .deleted_num_nodes = 0,                          \
                 .cmp_fn = _cmp_fn,                               \
                 .fill_val = ALLOC_INIT(type, 1, _fill_val_fn()), \
             })

#endif