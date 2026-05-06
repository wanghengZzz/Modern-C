

#include <assert.h>
#include <locale.h>
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
    {"(a+|∂+)+c", "123131aaa∂∂c", 1},
    {"(a+|∂+)+c", "aaa∂∂", 0},
    {"(£*)*b", "£££b", 1},
    {"(£*)*b", "££££", 0},
    {"((abc)+\\d)+", "123dqwdasabcabc3abcabc9abcabc0", 1}
    {"^abcde", "abcdeasd", 1}};
  */

  setlocale(LC_ALL, "");

  wchar_t* sub_word = L"ß∂ß∂123∑´•£";
  size_t max_pattern_len = 200;
  size_t max_text_len = 1000;
  size_t max_match_len = 20;
  wchar_t pattern[max_pattern_len + 1];
  wchar_t text[max_text_len + 1];
  wchar_t match_str[max_match_len + 1];
  memset(pattern, 0, sizeof(pattern));
  memset(text, 0, sizeof(text));
  memset(match_str, 0, sizeof(match_str));
  char const* input_err = "fgets failed\n";

  size_t match = 0;
  size_t cnt = 0;

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

    match = wcstoul(match_str, 0, 0);

    printf("Parsing Pattern: %ls\n", pattern);
    printf("---------------------------\n");
    const wchar_t* ptr = pattern;
    ASTNode* root = complie(&ptr);
    if (root) {
      dump_ast(root, 0);
    } else {
      printf("Failed to parse pattern.\n");
    }

    cnt++;
    const wchar_t *start, *end, *cur;
    deque* ans_pos = ALLOC_DEQUE(10, sizeof(pos_t));
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
    printf("Test case %zu: %ls\n", cnt, text);
    printf("Output: %s, correct ans: %s, Occurences: %zu\n",
           (occurences ? "match" : "unmatch"), (!match ? "unmatch" : "match"),
           occurences);

    blob_t* b = ALLOC_BLOB(wchar_t, wcslen(text) + wcslen(sub_word), text);
    INIT_LIST_HEAD(&(b->list));
    while (!DEQUE_IS_EMPTY(ans_pos)) {
      pos_t p = DEQUE_FRONT(ans_pos, pos_t);
      replace_word(&(b->list), p.start, p.n, sub_word, wcslen(sub_word));
      DEQUE_POP_FRONT(ans_pos);
    }
    free_deque(ans_pos);
    if (occurences) {
      printf("\nReplace: \n");
      printf("Replace String: %ls\n", sub_word);
      printf("Modified string: %ls\n", b->value);
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