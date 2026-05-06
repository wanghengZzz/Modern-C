#ifndef __AST_H_
#define __AST_H_

#include <ctype.h>
#include <stdint.h>

#include "utils.h"

typedef enum {
  NODE_CHAR,           /* 'a' */
  NODE_STAR,           /* '*' (Kleene star) */
  NODE_PLUS,           /* '+' (One or more) */
  NODE_QUESTION,       /* '?' (Zero or one) */
  NODE_DOT,            /* '.' (Any character) */
  NODE_PIPE,           /* '|' (Alternation) */
  NODE_GROUP,          /* '(...)' (Subexpression) */
  NODE_BRACES,         /* '{m,n}' (Quantifier) */
  NODE_CHAR_CLASS,     /* '[...]' (Character set) */
  NODE_INV_CHAR_CLASS, /* '[^...]' (Inverted set) */
  NODE_CARET,          /* '^' (Start of line) */
  NODE_DOLLAR,         /* '$' (End of line) */
  NODE_DIGIT,          /* '\d' ([0-9]) */
  NODE_NOT_DIGIT,      /* '\D' ([^0-9]) */
  NODE_ALPHA,          /* '\w' ([a-zA-Z0-9_]) */
  NODE_NOT_ALPHA,      /* '\W' ([^a-zA-Z0-9_]) */
  NODE_WHITESPACE,     /* '\s' (Whitespace) */
  NODE_NOT_WHITESPACE, /* '\S' (Non-whitespace) */
  NODE_CONCAT,         /* 'ab' (Sequence) */
} NodeType;

typedef struct ASTNode ASTNode;
typedef struct CHset CHset;

struct CHset {
  uint8_t ch_set[16];
};

struct ASTNode {
  NodeType type;
  union {
    unsigned char ch;
    ASTNode *child;
    CHset *cs;
    struct {
      ASTNode *left;
      ASTNode *right;
    };
    struct {
      int l;
      int r;
    };
  } data;
};

ASTNode *complie(const char *pattern[static 1]);

inline static void add_char(char c, CHset cs[static 1]) {
  cs->ch_set[c >> 3] |= (1u << (c & 0x07));
}

inline static int is_in_charset(char c, CHset cs[static 1]) {
  return cs->ch_set[c >> 3] & (1u << (c & 0x07));
}

inline static int check_char_range(char l, char r, char c) {
  return c >= l && c <= r;
}

int parse_int(const char *pattern[static 1]);

#define INIT_CHset(str, cs)                                                   \
  ({                                                                          \
    for (size_t __i = 0; __i < strlen(str); ++__i) add_char((str)[__i], &cs); \
  })

void dump_ast(ASTNode node[static 1], size_t level);

typedef int (*Cont)(const char *text, void *data) __attribute__((nonnull(2)));

typedef struct {
  ASTNode *right;
  Cont cont;
  void *data;
} ConcatCtx;

typedef struct {
  ASTNode *child;
  const char *prev;
  Cont cont;
  void *data;
} StarCtx;

typedef struct {
  ASTNode *child;
  int lo, hi, cur;
  Cont cont;
  void *data;
} BracesCtx;

typedef struct {
  const char **end;
} FinalCtx;

const char *regex_search(ASTNode root[restrict static 1],
                         const char *restrict text,
                         const char *match_end[restrict static 1]);
void free_ast(ASTNode *node);

#endif