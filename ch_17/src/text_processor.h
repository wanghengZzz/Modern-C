#ifndef __TEXT_PREPROCESSOR_H_
#define __TEXT_PREPROCESSOR_H_

#include <string.h>
#include <wchar.h>

#include "doubly_list.h"
#include "utils.h"
#ifndef __STDC_ISO_10646__
#error "wchar_t_wide_characters_have_to_be_Unicode_code_points"
#endif
#ifdef __STDC_MB_MIGHT_NEQ_WC__
#error "basic_character_codes_must_agree_on_char_and_wchar_t"
#endif

typedef struct {
  size_t max_len;
  wchar_t *const value;
  list_head list;
} blob_t;

typedef struct {
  size_t num_blobs;
  list_head head;
} text_processor_t;

#define ALLOC_BLOB(type, len, data)                                   \
  ALLOC_INIT(blob_t, 1,                                               \
             {                                                        \
                 .max_len = len,                                      \
                 .value = (type *)memdup(data, (len) * sizeof(type)), \
                 .list =                                              \
                     {                                                \
                         .prev = NULL,                                \
                         .next = NULL,                                \
                     },                                               \
             })

text_processor_t *alloc_texts(size_t n, size_t max_len,
                              wchar_t const *const *texts);

#define ALLOC_TEXTS(n, max_len, data) alloc_texts(n, max_len, data)

wchar_t *merge_two_texts(wchar_t *const t1, wchar_t *const t2);

int merge_two_blobs(list_head *b1, list_head *b2);

int split_a_blob(list_head *h, int pos);

void merge_blobs_to_lines(text_processor_t *tp);

int free_blob(blob_t *b);

int search_word(blob_t *str, const wchar_t *word);

void replace_word(list_head *str, const wchar_t *word, size_t n,
                  const wchar_t *sub_word, size_t m);

static inline size_t mbsrlen(char const **restrict s) {
  return mbsrtowcs(nullptr, s, 0, 0);
}

typedef struct {
  const wchar_t *start;
  size_t n;
} pos_t;

wchar_t *fgets_safe(wchar_t s[restrict], size_t n, FILE *restrict stream);

#endif