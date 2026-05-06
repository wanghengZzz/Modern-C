

#include <assert.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "src/ast.h"
#include "src/text_processor.h"

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

  printf("\nReplace: \n");
  char *str_copy = replace_word((&(tp->head))->next, "ger", "erveryday");
  printf("Modified string: %s\n",
         list_entry((&(tp->head))->next, blob_t, list)->value);
  if (!str_copy) free(str_copy);
  list_for_each_entry_safe(b1, b2, &tp->head, list) {
    printf("%s | ", b1->value);
  }
  printf("\n--------------------------------------\n");
  if (str_copy) free(str_copy);
  if (tp) free_text_processor(tp);
  static struct RegTests {
    const char *pattern;
    const char *text;
    int match;
  } test_cases[] = {
      {"[a-zA-Z0-9]+@[a-zA-Z]+\\.[a-zA-Z]+", "__##__user@example.com", 1},
      {"(a+|b+)+c", "123131aaabbc", 1},
      {"(a+|b+)+c", "aaabb", 0},
      {"(a*)*b", "aaab", 1},
      {"(a*)*b", "aaaa", 0},
      {"^abcde", "abcdeasd", 1}};

  for (size_t i = 0; i < ARRAY_SIZE(test_cases); ++i) {
    printf("Parsing Pattern: %s\n", test_cases[i].pattern);
    printf("---------------------------\n");
    const char *ptr = test_cases[i].pattern;
    ASTNode *root = complie(&ptr);
    if (root) {
      dump_ast(root, 0);
    } else {
      printf("Failed to parse pattern.\n");
    }
    const char *start, *end;

    start = regex_search(root, test_cases[i].text, &end);
    printf("Test case %zu: %s\n", i + 1, test_cases[i].text);
    printf("Output: %s, correct ans: %s\n", (start ? "match" : "unmatched"),
           (test_cases[i].match ? "match" : "unmatched"));
    if (start) {
      printf("matched: %.*s\n", (int)(end - start), start);
    }
    free_ast(root);
  }

  return 0;
}