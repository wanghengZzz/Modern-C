#ifndef __TEXT_PREPROCESSOR_H_
#define __TEXT_PREPROCESSOR_H_

#include <string.h>

#include "doubly_list.h"
#include "utils.h"

typedef struct {
  size_t max_len;
  char *const value;
  list_head list;
} blob_t;

typedef struct {
  size_t num_blobs;
  list_head head;
} text_processor_t;

#define ALLOC_INIT_BLOB(type, len, data)                              \
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

#define ALLOC_BLOB(type, len)                                     \
  ALLOC_INIT(blob_t, 1,                                           \
             {                                                    \
                 .max_len = len,                                  \
                 .value = (type *)calloc_safe(len, sizeof(type)), \
                 .list =                                          \
                     {                                            \
                         .prev = NULL,                            \
                         .next = NULL,                            \
                     },                                           \
             })

text_processor_t *alloc_texts(size_t n, size_t max_len,
                              char const *const *texts);

#define ALLOC_TEXTS(n, max_len, data) alloc_texts(n, max_len, data)

char *merge_two_texts(char *const t1, char *const t2);

int merge_two_blobs(list_head *b1, list_head *b2);

int split_a_blob(list_head *h, int pos);

void merge_blobs_to_lines(text_processor_t *tp);

int free_blob(blob_t *b);

void free_text_processor(text_processor_t *tp);

#endif