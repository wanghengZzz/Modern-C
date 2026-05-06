
#include "text_processor.h"

#include <stdio.h>
text_processor_t *alloc_texts(size_t n, size_t max_len,
                              char const *const *texts) {
  text_processor_t *tp = ALLOC_INIT(text_processor_t, 1,
                                    {
                                        .num_blobs = n,
                                        .head =
                                            {
                                                .prev = NULL,
                                                .next = NULL,
                                            },
                                    });
  INIT_LIST_HEAD(&tp->head);
  for (size_t i = 0; i < n; ++i) {
    list_add_tail(&(ALLOC_BLOB(char, max_len + 1, texts[i]))->list, &tp->head);
  }
  return tp;
}

char *merge_two_texts(char *const t1, char *const t2) {
  strcat(t1, t2);
  return t1;
}

int merge_two_blobs(list_head *h1, list_head *h2) {
  if (!h1 || !h2 || is_empty(h1) || is_empty(h2)) return 0;
  blob_t *b1 = list_entry(h1, blob_t, list);
  blob_t *b2 = list_entry(h2, blob_t, list);
  if (strlen(b1->value) + strlen(b2->value) > b1->max_len) return 0;
  merge_two_texts(b1->value, b2->value);
  list_del(h2);
  return free_blob(b2);
}

int split_a_blob(list_head *h, int pos) {
  if (!h || is_empty(h)) return 0;
  blob_t *b = list_entry(h, blob_t, list);
  if (pos < 0 || pos >= (int)strlen(b->value)) return 0;
  blob_t *split_b = ALLOC_BLOB(char, b->max_len, b->value + pos);
  b->value[pos] = '\0';
  list_add(&split_b->list, &b->list);
  return 1;
}

void merge_blobs_to_lines(text_processor_t *tp) {
  if (is_singular(&tp->head)) return;
  list_head *curr = tp->head.next, *next = curr->next;
  list_head *head = &tp->head;
  while (next != head) {
    blob_t *b1 = list_entry(curr, blob_t, list);
    blob_t *b2 = list_entry(next, blob_t, list);
    size_t val_1_len = strlen(b1->value);
    if (b1->value[val_1_len - 1] != '\n') {
      merge_two_blobs(&b1->list, &b2->list);
      next = curr->next;
    } else {
      curr = next;
      next = curr->next;
    }
  }
}

int free_blob(blob_t *b) {
  if (!b) return 0;
  if (b->value) free(b->value);
  free(b);
  return 1;
}

int search_word(blob_t *str, const char *word) {
  size_t n = strlen(str->value);
  size_t m = strlen(word);
  if (m > n) return -1;
  for (size_t i = 0; i < n; ++i) {
    size_t pos = i;
    if (pos + m - 1 < n)
      for (size_t j = 0; j < m && word[j] == str->value[pos++]; ++j);
    if (pos - i == m) return i;
  }
  return -1;
}

char *replace_word(list_head *str, const char *word, const char *sub_word) {
  blob_t *search_str = list_entry(str, blob_t, list);
  size_t n = strlen(word), m = strlen(sub_word);
  size_t old_n = strlen(search_str->value);
  size_t new_str_len = old_n + m - n;
  int pos;
  if ((pos = search_word(search_str, word)) == -1 ||
      new_str_len >= search_str->max_len - 1)
    return nullptr;
  memmove(CONTAINER_OFFSET(search_str->value, sizeof(char), pos + m),
          CONTAINER_OFFSET(search_str->value, sizeof(char), pos + n),
          old_n - pos - n);
  memcpy(CONTAINER_OFFSET(search_str->value, sizeof(char), pos), sub_word, m);
  return memdup(search_str->value, (new_str_len + 1) * sizeof(char));
}

void free_text_processor(text_processor_t *tp) {
  blob_t *b1, *b2;
  list_for_each_entry_safe(b1, b2, &tp->head, list) {
    if (b1->value) free(b1->value);
    free(b1);
  }
  free(tp);
}
