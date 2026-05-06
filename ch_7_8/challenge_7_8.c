#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/deque.h"
#include "src/disjoint_set.h"
#include "src/heap.h"
#define N 10

void dfs(int entry, int (*G)[N]) {
  int visit[N] = {};
  deque *dq = ALLOC_DEQUE(10, sizeof(typeof(G[0][0])));
  int pre = -1;
  visit[entry] = 1;
  DEQUE_PUSH_BACK(dq, entry);
  while (!DEQUE_IS_EMPTY(dq)) {
    int s = DEQUE_BACK(dq, typeof(G[0][0]));
    DEQUE_POP_BACK(dq);
    printf("%d -> %d\n", pre, s);
    pre = s;
    for (size_t adj = 0; adj < ARRAY_SIZE(G[s]); ++adj) {
      if (G[s][adj] && !visit[adj]) {
        DEQUE_PUSH_BACK(dq, adj);
        visit[adj] = 1;
      }
    }
  }
}

void bfs(int entry, int (*G)[N]) {
  int visit[N] = {};
  deque *dq = ALLOC_DEQUE(10, sizeof(typeof(G[0][0])));
  int pre = -1;
  DEQUE_PUSH_BACK(dq, entry);
  visit[entry] = 1;
  while (!DEQUE_IS_EMPTY(dq)) {
    int s = DEQUE_FRONT(dq, typeof(G[0][0]));
    DEQUE_POP_FRONT(dq);
    printf("%d -> %d\n", pre, s);
    pre = s;
    for (size_t adj = 0; adj < ARRAY_SIZE(G[s]); ++adj) {
      if (G[s][adj] && !visit[adj]) {
        DEQUE_PUSH_BACK(dq, adj);
        visit[adj] = 1;
      }
    }
  }
}

typedef struct {
  int from;
  int to;
  int w;
} edge;

int edge_increasing_cmp(void const *e1, void const *e2) {
  return ((edge *)e1)->w < ((edge *)e2)->w;
}

int edge_decreasing_cmp(void const *e1, void const *e2) {
  return ((edge *)e1)->w > ((edge *)e2)->w;
}

edge get_max_edge() {
  return (edge){
      .from = -1,
      .to = -1,
      .w = INT32_MAX,
  };
}

edge get_min_edge() {
  return (edge){
      .from = -1,
      .to = -1,
      .w = INT32_MIN,
  };
}

int kruskal(int (*G)[N]) {
  int tot = 0;
  deque *dq = ALLOC_DEQUE(N * N, sizeof(int));
  heap *min_hp = ALLOC_HEAP(edge, edge_increasing_cmp, get_max_edge);
  disjoint_set *ds = ALLOC_DISJOINT_SET(N + 1);
  tree_node *min_node = NEW_NODE(edge, -1, -1, get_max_edge().w);
  edge *min_edge = (edge *)min_node->data;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = i + 1; j < N; ++j) {
      if (G[i][j] && G[i][j] < min_edge->w) {
        min_edge->from = i;
        min_edge->to = j;
        min_edge->w = G[i][j];
      }
    }
  }
  if (min_edge->from == -1 || min_edge->to == -1) return 0;
  heap_insert(min_hp, min_node);

  while (!HEAP_IS_EMPTY(min_hp)) {
    tree_node *top = heap_get_top(min_hp);
    edge *e = (edge *)top->data;
    int p_e = find(ds, e->from), p_f = find(ds, e->to);
    if (ds->groups[p_e] == ds->groups[p_f]) {
      DEQUE_PUSH_BACK(dq, e->from);
      DEQUE_PUSH_BACK(dq, e->to);
    } else if (ds->groups[p_e] < ds->groups[p_f]) {
      DEQUE_PUSH_BACK(dq, e->from);
    } else {
      DEQUE_PUSH_BACK(dq, e->to);
    }
    if (is_same_set(ds, e->from, e->to)) {
      heap_pop(min_hp);
      continue;
    }
    disjoint_set_union(ds, e->from, e->to);
    printf("%d -> %d, w: %d\n", e->from, e->to, e->w);
    tot += e->w;
    heap_pop(min_hp);
    while (!DEQUE_IS_EMPTY(dq)) {
      int back = DEQUE_BACK(dq, int);
      DEQUE_POP_BACK(dq);
      for (int i = 0; i < N; ++i) {
        if (G[back][i] && !is_same_set(ds, back, i)) {
          heap_insert(min_hp, NEW_NODE(edge, back, i, G[back][i]));
        }
      }
    }
    printf("\n");
  }
  free_tree_nodes(min_hp->root);
  free_disjoint_set(ds);
  free_heap(min_hp);
  free_deque(dq);
  return tot;
}

size_t find_connected_components(int (*G)[N]) {
  disjoint_set *ds = ALLOC_DISJOINT_SET(N + 1);
  int visit[N] = {};
  deque *dq = ALLOC_DEQUE(N + 1, sizeof(typeof(G[0][0])));
  size_t comp_nums = 0;
  for (int i = 0; i < N; ++i) {
    if (!visit[i]) {
      int pre = i;
      visit[pre] = 1;
      DEQUE_PUSH_BACK(dq, pre);
      printf("Component %zu: \n", ++comp_nums);
      while (!DEQUE_IS_EMPTY(dq)) {
        int s = DEQUE_BACK(dq, typeof(G[0][0]));
        DEQUE_POP_BACK(dq);
        if (pre != s) {
          printf("%d -> %d\n", pre, s);
          disjoint_set_union(ds, pre, s);
        }

        pre = s;
        for (size_t adj = 0; adj < ARRAY_SIZE(G[s]); ++adj) {
          if (G[s][adj] && !visit[adj]) {
            DEQUE_PUSH_BACK(dq, adj);
            visit[adj] = 1;
          }
        }
      }
    }
  }
  free_disjoint_set(ds);
  free_deque(dq);
  return comp_nums;
}

void print_graph(int (*G)[N]) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) printf("%d ", G[i][j]);
    printf("\n");
  }
}

void floyld_warshall(int (*G)[N], int (*ret)[N]) {
  int dp[N][N];
  for (int i = 0; i < N; ++i)
    for (int j = 0; j < N; ++j)
      dp[i][j] = (G[i][j] || i == j) ? G[i][j] : INT32_MAX;
  for (int k = 0; k < N; ++k)
    for (int i = 0; i < N; ++i)
      for (int j = 0; j < N; ++j)
        dp[i][j] = (dp[i][k] != INT32_MAX && dp[k][j] != INT32_MAX &&
                    dp[i][k] + dp[k][j] < dp[i][j])
                       ? dp[i][k] + dp[k][j]
                       : dp[i][j];
  printf("\n\nMin Distances Matrix:\n\n");
  print_graph(dp);
  memcpy(ret, dp, sizeof(dp));
}

int main(void) {
  int G[N][N] = {
      {0, 4, 0, 0, 0, 0, 0, 8, 0, 0},  {4, 0, 8, 0, 0, 0, 0, 11, 0, 0},
      {0, 8, 0, 7, 0, 4, 0, 0, 2, 0},  {0, 0, 7, 0, 9, 14, 0, 0, 0, 0},
      {0, 0, 0, 9, 0, 10, 0, 0, 0, 0}, {0, 0, 4, 14, 10, 0, 2, 0, 0, 0},
      {0, 0, 0, 0, 0, 2, 0, 1, 6, 0},  {8, 11, 0, 0, 0, 0, 1, 0, 7, 0},
      {0, 0, 2, 0, 0, 0, 6, 7, 0, 3},  {0, 0, 0, 0, 0, 0, 0, 0, 3, 0}};
  int G2[N][N] = {
      // 0 1 2 3 4 5 6 7 8 9
      {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},  // 0
      {1, 0, 1, 0, 0, 0, 0, 0, 0, 0},  // 1
      {0, 1, 0, 1, 0, 0, 0, 0, 0, 0},  // 2
      {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},  // 3

      {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},  // 4
      {0, 0, 0, 0, 1, 0, 1, 0, 0, 0},  // 5
      {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},  // 6

      {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},  // 7
      {0, 0, 0, 0, 0, 0, 0, 1, 0, 1},  // 8
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 0}   // 9
  };
  printf("\nMin Spanning tree: %d\n", kruskal(G));
  printf("\nNum of Connected Components: %zu\n", find_connected_components(G2));

  int min_dis[N][N];
  floyld_warshall(G, min_dis);
  return 0;
}