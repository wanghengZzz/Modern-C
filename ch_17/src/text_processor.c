
#include "text_processor.h"

#include <stdio.h>
text_processor_t *alloc_texts(size_t n, size_t max_len,
                              wchar_t const *const *texts) {
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
    list_add_tail(&(ALLOC_BLOB(wchar_t, max_len + 1, texts[i]))->list,
                  &tp->head);
  }
  return tp;
}

wchar_t *merge_two_texts(wchar_t *const t1, wchar_t *const t2) {
  wcscat(t1, t2);
  return t1;
}

int merge_two_blobs(list_head *h1, list_head *h2) {
  if (!h1 || !h2 || is_empty(h1) || is_empty(h2)) return 0;
  blob_t *b1 = list_entry(h1, blob_t, list);
  blob_t *b2 = list_entry(h2, blob_t, list);
  if (wcslen(b1->value) + wcslen(b2->value) > b1->max_len) return 0;
  merge_two_texts(b1->value, b2->value);
  list_del(h2);
  return free_blob(b2);
}

int split_a_blob(list_head *h, int pos) {
  if (!h || is_empty(h)) return 0;
  blob_t *b = list_entry(h, blob_t, list);
  if (pos < 0 || pos >= (int)wcslen(b->value)) return 0;
  blob_t *split_b = ALLOC_BLOB(wchar_t, b->max_len, b->value + pos);
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
    size_t val_1_len = wcslen(b1->value);
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

int search_word(blob_t *str, const wchar_t *word) {
  size_t n = wcslen(str->value);
  size_t m = wcslen(word);
  if (m > n) return -1;
  for (size_t i = 0; i < n; ++i) {
    size_t pos = i;
    if (pos + m - 1 < n)
      for (size_t j = 0; j < m && word[j] == str->value[pos++]; ++j);
    if (pos - i == m) return i;
  }
  return -1;
}

void replace_word(list_head *str, const wchar_t *word, size_t n,
                  const wchar_t *sub_word, size_t m) {
  blob_t *search_str = list_entry(str, blob_t, list);
  size_t old_n = wcslen(search_str->value);
  size_t new_str_len = old_n + m - n;
  int pos;
  if ((pos = search_word(search_str, word)) == -1 ||
      new_str_len >= search_str->max_len - 1)
    return;
  memmove(CONTAINER_OFFSET(search_str->value, sizeof(wchar_t), pos + m),
          CONTAINER_OFFSET(search_str->value, sizeof(wchar_t), pos + n),
          (old_n - pos - n + 1) * sizeof(wchar_t));
  memcpy(CONTAINER_OFFSET(search_str->value, sizeof(wchar_t), pos), sub_word,
         m * sizeof(wchar_t));
}

wchar_t *fgets_safe(wchar_t s[restrict], size_t n, FILE *restrict stream) {
  if (!stream || !n) return 0;

  for (size_t i = 0; i < n; ++i) {
    int val = fgetwc(stream);
    switch (val) {
      case EOF:
        if (feof(stream)) {
          s[i] = 0;
          return s;
        } else {
          return 0;
        }
      case '\n':
        s[i] = '\0';
        return s;
      default:
        s[i] = val;
    }
  }
  s[n] = '\0';
  return s;
}
