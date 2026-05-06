/* Linux-like circular doubly-linked list implementation */
/* Reference by jserv: https://github.com/sysprog21/lab0-c */

#ifndef __DOUBLY_LIST_H_
#define __DOUBLY_LIST_H_

#include <stddef.h>

/**
 * Feature detection for 'typeof':
 * - Supported as a GNU extension in GCC/Clang.
 * - Part of C23 standard (ISO/IEC 9899:2024).
 *
 * Reference: https://gcc.gnu.org/onlinedocs/gcc/Typeof.html
 */
#if defined(__GNUC__) || defined(__clang__) ||         \
    (defined(__STDC__) && defined(__STDC_VERSION__) && \
     (__STDC_VERSION__ >= 202311L)) /* C23 ?*/
#define __LIST_HAVE_TYPEOF 1
#else
#define __LIST_HAVE_TYPEOF 0
#endif

typedef struct list_head list_head;
struct list_head {
  struct list_head *prev;
  struct list_head *next;
};

#ifndef container_of
#if __LIST_HAVE_TYPEOF
#define container_of(ptr, type, member)                  \
  __extension__({                                        \
    const typeof(((type *)0)->member) *__member = (ptr); \
    (type *)((char *)__member - offsetof(type, member)); \
  })
#else
#define container_of(ptr, type, member) \
  ((type *)((char *)(ptr) - offsetof(type, member)))
#endif
#endif

#define LIST_HEAD(head) struct list_head head = {&(head), &(head)}

#define list_for_each_safe(node, safe, head)                   \
  for (node = (head)->next, safe = node->next; node != (head); \
       node = safe, safe = node->next)

#define list_entry(node, type, member) container_of(node, type, member)

#if __LIST_HAVE_TYPEOF
#define list_for_each_entry_safe(entry, safe, head, member)          \
  for (entry = list_entry((head)->next, typeof(*entry), member),     \
      safe = list_entry(entry->member.next, typeof(*entry), member); \
       &entry->member != (head); entry = safe,                       \
      safe = list_entry(safe->member.next, typeof(*entry), member))
#else
#define list_for_each_entry_safe(entry, safe, head, member)      \
  for (entry = safe = (void *)1; sizeof(struct { int i : -1; }); \
       ++(entry), ++(safe))
#endif

static inline void INIT_LIST_HEAD(struct list_head *head) {
  head->next = head;
  head->prev = head;
}

static inline void list_add(list_head *node, list_head *head) {
  list_head *next = head->next;
  node->next = next;
  next->prev = node;
  head->next = node;
  node->prev = head;
}

static inline void list_add_tail(list_head *node, list_head *head) {
  list_head *prev = head->prev;
  prev->next = node;
  node->prev = prev;
  node->next = head;
  head->prev = node;
}

static inline int is_empty(list_head *head) { return head == head->next; }

static inline int is_singular(list_head *head) {
  if (!is_empty(head) && head->next == head->prev) return 1;
  return 0;
}

static inline void list_del(list_head *node) {
  node->next->prev = node->prev;
  node->prev->next = node->next;
}

#endif