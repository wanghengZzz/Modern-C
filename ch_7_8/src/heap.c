

#include "heap.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

tree_node *find_leftest_node(tree_node *root) {
  if (!root || !root->left) return root;
  return find_leftest_node(root->left);
}

tree_node *find_next_parent(tree_node *parent, tree_node *root, size_t idx,
                            size_t pos) {
  if (pos > 31) return parent;
  if (BITS_32(pos) & idx) {
    return find_next_parent(root, root->right, idx, pos + 1);
  } else {
    return find_next_parent(root, root->left, idx, pos + 1);
  }
}

int heap_bottom_up_update(heap *hp, tree_node *node) {
  if (!node || node == node->parent) return 0;
  while (node->data != node->parent->data && hp->cmp_fn(node->data, node->parent->data)) {
    swap(node->data, node->parent->data, node->data_sz);
    node = node->parent;
  }
  return 1;
}

int heap_top_down_update(heap *hp, tree_node *node) {
  if (!hp->root) return 0;
  if (!node) return 1;
  tree_node *updated_node = NULL;
  updated_node = node->left ? node->left : node->right;
  if (node->left && node->right &&
      hp->cmp_fn(node->right->data, node->left->data)) {
    updated_node = node->right;
  }
  if (updated_node && hp->cmp_fn(updated_node->data, node->data)) {
    swap(updated_node->data, node->data, node->data_sz);
    return heap_top_down_update(hp, updated_node);
  }
  return 1;
}

int heap_insert(heap *hp, tree_node *node) {
  hp->num_nodes++;

  if (hp->deleted_num_nodes) {
    size_t pos = 0;
    while (!(BITS_32(pos++) & hp->num_nodes));
    tree_node *last_parent =
        find_next_parent(hp->root, hp->root, hp->num_nodes, pos);
    tree_node *target = NULL;
    if (last_parent->left &&
        !memcmp(last_parent->left->data, hp->fill_val, node->data_sz))
      target = last_parent->left;
    else if (last_parent->right &&
             !memcmp(last_parent->right->data, hp->fill_val, node->data_sz))
      target = last_parent->right;
    else if (!memcmp(last_parent->data, hp->fill_val, node->data_sz))
      target = last_parent;

    if (target) {
      hp->deleted_num_nodes--;
      memcpy(target->data, node->data, node->data_sz);
      free(node->data);
      return heap_bottom_up_update(hp, target);
    }
  }

  tree_node *new_node = memdup(node, sizeof(tree_node));
  hp->allocated_nodes++;
  if (!new_node) return 0;

  if (!hp->root) {
    hp->root = new_node;
    hp->root->parent = hp->root;
    hp->last_parent = hp->root;
    return 1;
  }
  if (!hp->last_parent->left) {
    hp->last_parent->left = new_node;
    hp->last_parent->left->parent = hp->last_parent;
  } else if (!hp->last_parent->right) {
    hp->last_parent->right = new_node;
    hp->last_parent->right->parent = hp->last_parent;
  } else {
    tree_node *last_parent = NULL;
    if (IS_POW2(hp->allocated_nodes + 1)) {
      last_parent = find_leftest_node(hp->root);
    } else {
      size_t pos = 0;
      while (!(BITS_32(pos++) & hp->allocated_nodes));
      last_parent = find_next_parent(hp->root, hp->root, hp->allocated_nodes, pos);
    }
    last_parent->left = new_node;
    hp->last_parent = last_parent;
    new_node->parent = last_parent;
  }
  return heap_bottom_up_update(hp, new_node);
}

int heap_pop(heap *hp) {
  if (!hp || !hp->root ||
      !memcmp(hp->root->data, hp->fill_val, hp->root->data_sz))
    return 0;

  hp->num_nodes--;
  hp->deleted_num_nodes++;
  memcpy(hp->root->data, hp->fill_val, hp->root->data_sz);
  return heap_top_down_update(hp, hp->root);
}

void *heap_get_top(heap *hp) { return hp->root; }

int heap_is_empty(heap *hp) {
  return !hp || !memcmp(hp->root->data, hp->fill_val, hp->root->data_sz);
}

void free_tree_nodes(tree_node *node) {
  if (!node) return;
  if (node->left) free_tree_nodes(node->left);
  if (node->right) free_tree_nodes(node->right);
  if (node->data) {
    free(node->data);
    node->data = NULL;
  }
  free(node);
  node = NULL;
}

int free_heap(heap *hp) {
  if (!hp) return 0;

  if (hp->fill_val) {
    free(hp->fill_val);
    hp->fill_val = NULL;
  }
  free(hp);
  hp = NULL;
  return 1;
}
