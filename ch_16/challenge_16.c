

#include <assert.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "src/ast.h"
#include "src/deque.h"
#include "src/text_processor.h"

int main(void) {
  /*
    Test Cases:
    {"[a-zA-Z0-9]+@[a-zA-Z]+\\.[a-zA-Z]+",
    "__#user@example.com#__hello@example.com", 1},
    {"(a+|b+)+c", "123131aaabbc", 1},
    {"(a+|b+)+c", "aaabb", 0},
    {"(a*)*b", "aaab", 1},
    {"(a*)*b", "aaaa", 0},
    {"^abcde", "abcdeasd", 1}};
    {"((abc)+\\d)+", "123dqwdasabcabc3abcabc9abcabc0", 1}
  */

  size_t max_pattern_len = 200;
  size_t max_text_len = 1000;
  size_t max_match_len = 20;
  size_t cnt = 0;

  const char *sub_word = "SUBWORD";
  char pattern[max_pattern_len + 1];
  char text[max_text_len + 1];
  char match_str[max_match_len + 1];
  memset(pattern, 0, max_pattern_len + 1);
  memset(text, 0, max_text_len + 1);
  memset(match_str, 0, max_match_len + 1);
  char const *input_err = "fgets failed\n";
  size_t match = 0;
  while (1) {
    printf("Input regex pattern:\n>");
    if (!fgets_safe(pattern, max_pattern_len, stdin)) {
      fprintf(stderr, "Error: %s\n", input_err);
      break;
    }
    printf("Input text:\n>");
    if (!fgets_safe(text, max_text_len, stdin)) {
      fprintf(stderr, "Error: %s\n", input_err);
      break;
    }
    printf("Input ans of given regex and text (0:unmatch, 1:match):\n>");
    if (!fgets_safe(match_str, max_match_len, stdin)) {
      fprintf(stderr, "Error: %s\n", input_err);
      break;
    }
    match = strtoul(match_str, 0, 0);

    printf("Parsing Pattern: %s\n", pattern);
    printf("---------------------------\n");
    const char *ptr = pattern;
    ASTNode *root = complie(&ptr);
    if (root) {
      dump_ast(root, 0);
    } else {
      printf("Failed to parse pattern.\n");
    }

    cnt++;
    const char *start, *end, *cur;
    deque *ans_pos = ALLOC_DEQUE(10, sizeof(pos_t));
    size_t occurences = 0;
    cur = text;
    do {
      start = regex_search(root, cur, &end);
      if (start) {
        DEQUE_PUSH_BACK(ans_pos,
                        ((pos_t){.n = (size_t)(end - start), .start = start}));
        cur = end + 1;
        occurences++;
      }
    } while (start);
    free_ast(root);
    printf("Test case %zu: %s\n", cnt, text);
    printf("Output: %s, correct ans: %s, Occurences: %zu\n",
           (occurences ? "match" : "unmatch"), (!match ? "unmatch" : "match"),
           occurences);

    blob_t *b = ALLOC_BLOB(char, strlen(text) + strlen(sub_word), text);
    INIT_LIST_HEAD(&(b->list));
    while (!DEQUE_IS_EMPTY(ans_pos)) {
      pos_t p = DEQUE_FRONT(ans_pos, pos_t);
      printf("%zu\n", p.n);
      replace_word(&(b->list), p.start, p.n, sub_word, strlen(sub_word));
      DEQUE_POP_FRONT(ans_pos);
    }
    free_deque(ans_pos);
    if (occurences) {
      printf("\nReplace: \n");
      printf("Replace String: %s\n", sub_word);
      printf("Modified string: %s\n", b->value);
    }
    free_blob(b);
    printf("Stop ? (press 'q' to exit, 'c' to continue):\n>");
    if (!fgets_safe(pattern, 2, stdin)) {
      fprintf(stderr, "Error: %s\n", input_err);
      break;
    }
    if (pattern[0] == 'q') break;
    printf("\033[H\033[2J");
    fflush(stdout);
  }
  return EXIT_SUCCESS;
}