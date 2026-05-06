

#include <assert.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "src/text_processor.h"

void get_terminal_size(int *rows, int *cols) {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  *rows = w.ws_row;
  *cols = w.ws_col;
}

void print_subject_to_screen(text_processor_t *tp, int win_cols) {
  blob_t *b1, *b2;
  int tot_ch = 0;
  printf("\nPrint texts fitting screen's size: \n");
  list_for_each_entry_safe(b1, b2, &tp->head, list) {
    for (size_t i = 0; i < strlen(b1->value); ++i) {
      if (b1->value[i] == '\n') continue;
      tot_ch += 1;
      putchar(b1->value[i]);
      if (!(tot_ch % win_cols)) {
        putchar('\n');
      }
    }
  }
  printf("\n--------------------------------------\n");
}

int main(int argc, char *argv[argc + 1]) {
  char const *const strings[] = {
      "zeg",  "eric\n", "juice",   "apple\n", "pineapple\n", ",eabcas",
      "asda", "Bad ",   "Hello\n", "end ",    "end\n",
  };
  size_t max_len = 100;
  if (argc > 1) assert(sscanf(argv[1], "%zu", &max_len) == 1);
  size_t n = ARRAY_SIZE(strings);
  text_processor_t *tp = ALLOC_TEXTS(n, max_len, strings);

  blob_t *b1, *b2;

  printf("\nOriginal: \n");
  list_for_each_entry_safe(b1, b2, &tp->head, list) {
    printf("%s | ", b1->value);
  }
  printf("\n--------------------------------------\n");

  printf("\nMerge blobs to line util reach line break: \n");
  merge_blobs_to_lines(tp);
  list_for_each_entry_safe(b1, b2, &tp->head, list) {
    printf("%s | ", b1->value);
  }
  printf("\n--------------------------------------\n");

  printf("\nTest split function is able to work: \n");
  split_a_blob((&tp->head)->next->next->next, 6);
  split_a_blob((&tp->head)->next->next->next->next->next, 8);
  list_for_each_entry_safe(b1, b2, &tp->head, list) {
    printf("%s | ", b1->value);
  }
  printf("\n--------------------------------------\n");

  printf("\nMerge again: \n");
  merge_blobs_to_lines(tp);
  list_for_each_entry_safe(b1, b2, &tp->head, list) {
    printf("%s | ", b1->value);
  }
  printf("\n--------------------------------------\n");

  int win_rows = 5;

  // if you want to get window's size
  // int win_rows = 5, win_cols = 5;
  // get_terminal_size(&win_rows, &win_cols);

  print_subject_to_screen(tp, win_rows);
  free_text_processor(tp);
  return 0;
}