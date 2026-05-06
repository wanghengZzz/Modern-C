
#include "ast.h"

static const char* NODE_NAMES[] = {
    "CHAR",       "STAR",           "DOT",        "PLUS",
    "PIPE",       "GROUP",          "CHAR_CLASS", "INV_CHAR_CLASS",
    "CARET",      "DOLLAR",         "QUESTION",   "BRACES",
    "DIGIT",      "NOT_DIGIT",      "ALPHA",      "NOT_ALPHA",
    "WHITESPACE", "NOT_WHITESPACE", "CONCAT"};

int parse_int(const char* pattern[static 1]) {
  int val = 0;
  while (check_char_range('0', '9', **pattern)) {
    val = val * 10 + (**pattern - '0');
    (*pattern)++;
  }
  return val;
}

static int parse_char_class(const char* pattern[restrict static 1],
                            CHset cs[restrict static 1]) {
  while (**pattern && **pattern != ']') {
    unsigned char c = (unsigned char)**pattern;

    if (c == '\\') {
      (*pattern)++;
      if (!**pattern) return 0;
      add_char(**pattern, cs);
      (*pattern)++;
      continue;
    }

    if (*(*pattern + 1) == '-' && *(*pattern + 2) && *(*pattern + 2) != ']') {
      unsigned char lo = c;
      unsigned char hi = (unsigned char)*(*pattern + 2);
      if (lo > hi) return 0;
      for (unsigned char s = lo; s <= hi; s++) add_char(s, cs);
      (*pattern) += 3;
      continue;
    }

    add_char(c, cs);
    (*pattern)++;
  }

  if (**pattern != ']') return 0;
  (*pattern)++;
  return 1;
}

static ASTNode* parse_ast_level_1(const char* pattern[static 1]);
static ASTNode* parse_ast_level_2(const char* pattern[static 1]);
static ASTNode* parse_ast_level_3(const char* pattern[restrict static 1],
                                  CHset cs[static 1]);
static ASTNode* parse_ast_level_4(const char* pattern[static 1]);

static ASTNode* parse_ast_level_1(const char* pattern[static 1]) {
  ASTNode* left = parse_ast_level_2(pattern);
  if (left && **pattern == '|') {
    (*pattern)++;
    ASTNode* right = parse_ast_level_1(pattern);
    ASTNode* node = malloc_safe(sizeof(ASTNode));
    node->type = NODE_PIPE;
    node->data.left = left;
    node->data.right = right;
    return node;
  }
  return left;
}

static ASTNode* parse_ast_level_2(const char* pattern[static 1]) {
  CHset cs = {0};
  INIT_CHset("*+?{", cs);
  ASTNode* left = parse_ast_level_3(pattern, &cs);
  if (left && **pattern && **pattern != '|' && **pattern != ')') {
    ASTNode* right = parse_ast_level_2(pattern);
    ASTNode* node = malloc_safe(sizeof(ASTNode));
    node->type = NODE_CONCAT;
    node->data.left = left;
    node->data.right = right;
    return node;
  }
  return left;
}

static ASTNode* parse_ast_level_3(const char* pattern[static 1],
                                  CHset cs[static 1]) {
  ASTNode* node = parse_ast_level_4(pattern);
  if (!node) return nullptr;
  while (**pattern && is_in_charset(**pattern, cs)) {
    ASTNode* parent = malloc_safe(sizeof(ASTNode));
    parent->data.child = node;
    switch (**pattern) {
      case '*': {
        parent->type = NODE_STAR;
      } break;
      case '+': {
        parent->type = NODE_PLUS;
      } break;
      case '?': {
        parent->type = NODE_QUESTION;
      } break;
      case '{': {
        (*pattern)++;
        int range_min = parse_int(pattern);
        int range_max = range_min;

        if (**pattern == ',') {
          (*pattern)++;
          range_max =
              check_char_range('0', '9', **pattern) ? parse_int(pattern) : -1;
        }

        if (**pattern != '}') return nullptr;

        parent->type = NODE_BRACES;
        parent->data.l = range_min;
        parent->data.r = range_max;
      } break;
    }
    (*pattern)++;
    node = parent;
  }
  return node;
}

static ASTNode* parse_ast_level_4(const char* pattern[static 1]) {
  ASTNode* node = malloc_safe(sizeof(ASTNode));
  switch (**pattern) {
    case '[': {
      (*pattern)++;
      node->data.cs = malloc_safe(sizeof(CHset));

      if (**pattern == '^') {
        (*pattern)++;
        node->type = NODE_INV_CHAR_CLASS;
      } else {
        node->type = NODE_CHAR_CLASS;
      }

      if (!parse_char_class(pattern, node->data.cs)) {
        free(node->data.cs);
        free(node);
        return nullptr;
      }
    } break;
    case '(': {
      (*pattern)++;
      free(node);
      node = parse_ast_level_1(pattern);
      if (**pattern == ')')
        (*pattern)++;
      else
        return nullptr;
      ASTNode* parent = malloc_safe(sizeof(ASTNode));
      parent->data.child = node;
      parent->type = NODE_GROUP;
      node = parent;
    } break;
    case '\\': {
      (*pattern)++;
      switch (**pattern) {
        case 'd':
          node->type = NODE_DIGIT;
          break;
        case 'D':
          node->type = NODE_NOT_DIGIT;
          break;
        case 'w':
          node->type = NODE_ALPHA;
          break;
        case 'W':
          node->type = NODE_NOT_ALPHA;
          break;
        case 's':
          node->type = NODE_WHITESPACE;
          break;
        case 'S':
          node->type = NODE_NOT_WHITESPACE;
          break;
        default:
          node->type = NODE_CHAR;
          node->data.ch = **pattern;
          break;
      }
      (*pattern)++;
    } break;
    case '^': {
      node->type = NODE_CARET;
      (*pattern)++;
    } break;
    case '$': {
      node->type = NODE_DOLLAR;
      (*pattern)++;
    } break;
    case '.': {
      node->type = NODE_DOT;
      (*pattern)++;
    } break;
    default: {
      node->type = NODE_CHAR;
      node->data.ch = **pattern;
      (*pattern)++;
    } break;
  }
  return node;
}

ASTNode* complie(const char* pattern[static 1]) {
  return parse_ast_level_1(pattern);
}

void dump_ast(ASTNode node[static 1], size_t level) {
  for (size_t i = 0; i < level; i++) printf("  ");

  printf("[%s]", NODE_NAMES[node->type]);

  switch (node->type) {
    case NODE_CHAR:
      printf(" -> '%c'", node->data.ch);
      break;
    case NODE_BRACES:
      printf(" -> {%d, %d}", node->data.l, node->data.r);
      break;
    default:
      break;
  }
  printf("\n");

  if (node->type == NODE_PIPE || node->type == NODE_CONCAT) {
    dump_ast(node->data.left, level + 1);
    dump_ast(node->data.right, level + 1);
  } else if (node->type == NODE_STAR || node->type == NODE_PLUS ||
             node->type == NODE_QUESTION || node->type == NODE_GROUP ||
             node->type == NODE_BRACES) {
    dump_ast(node->data.child, level + 1);
  }
}

void free_ast(ASTNode* node) {
  if (!node) return;
  if (node->type == NODE_CHAR_CLASS || node->type == NODE_INV_CHAR_CLASS) {
    if (node->data.cs) free(node->data.cs);
  } else if (node->type == NODE_PIPE || node->type == NODE_CONCAT) {
    free_ast(node->data.left);
    free_ast(node->data.right);
  } else if (node->type == NODE_STAR || node->type == NODE_PLUS ||
             node->type == NODE_QUESTION || node->type == NODE_GROUP ||
             node->type == NODE_BRACES) {
    free_ast(node->data.child);
  }
  free(node);
}

int match(ASTNode node[restrict static 1], const char* restrict text, Cont cont,
          void* restrict data);
static int match_star(ASTNode child[static 1], const char* restrict text,
                      Cont cont, void* restrict data);

static int concat_cont(const char* restrict text, void* restrict data) {
  ConcatCtx* c = (ConcatCtx*)data;
  return match(c->right, text, c->cont, c->data);
}

static int star_inner_cont(const char* restrict text, void* restrict data) {
  StarCtx* c = (StarCtx*)data;
  if (text == c->prev) return c->cont(text, c->data);
  return match_star(c->child, text, c->cont, c->data);
}

static int match_braces(ASTNode child[static 1], int lo, int hi, int cur,
                        const char* restrict text, Cont cont,
                        void* restrict data);

static int braces_inner_cont(const char* restrict text, void* restrict data) {
  BracesCtx* c = (BracesCtx*)data;
  return match_braces(c->child, c->lo, c->hi, c->cur + 1, text, c->cont,
                      c->data);
}

static int match_star(ASTNode child[static 1], const char* restrict text,
                      Cont cont, void* restrict data) {
  StarCtx ctx = {child, text, cont, data};
  if (match(child, text, star_inner_cont, &ctx)) return 1;
  return cont(text, data);
}

static int match_braces(ASTNode child[static 1], int lo, int hi, int cur,
                        const char* restrict text, Cont cont,
                        void* restrict data) {
  if (hi != -1 && cur > hi) return 0;

  BracesCtx ctx = {child, lo, hi, cur, cont, data};

  if (cur < lo) {
    return match(child, text, braces_inner_cont, &ctx);
  }

  if (hi == -1 || cur < hi) {
    if (match(child, text, braces_inner_cont, &ctx)) return 1;
  }
  return cont(text, data);
}

int match(ASTNode node[restrict static 1], const char* restrict text, Cont cont,
          void* restrict data) {
  switch (node->type) {
    case NODE_CHAR:
      if (*text && *text == node->data.ch) return cont(text + 1, data);
      return 0;
    case NODE_DOT:
      if (*text && *text != '\n') return cont(text + 1, data);
      return 0;
    case NODE_DIGIT:
      if (*text && isdigit(*text)) return cont(text + 1, data);
      return 0;
    case NODE_NOT_DIGIT:
      if (*text && !isdigit(*text)) return cont(text + 1, data);
      return 0;
    case NODE_ALPHA:
      if (*text && (isalnum(*text) || *text == '_'))
        return cont(text + 1, data);
      return 0;
    case NODE_NOT_ALPHA:
      if (*text && !(isalnum(*text) || *text == '_'))
        return cont(text + 1, data);
      return 0;
    case NODE_WHITESPACE:
      if (*text && isspace(*text)) return cont(text + 1, data);
      return 0;
    case NODE_NOT_WHITESPACE:
      if (*text && !isspace(*text)) return cont(text + 1, data);
      return 0;
    case NODE_CARET:
      return cont(text, data);
    case NODE_DOLLAR:
      if (*text == '\0') return cont(text, data);
      return 0;
    case NODE_GROUP:
      return match(node->data.child, text, cont, data);
    case NODE_CONCAT: {
      ConcatCtx ctx = {node->data.right, cont, data};
      return match(node->data.left, text, concat_cont, &ctx);
    }
    case NODE_PIPE:
      return match(node->data.left, text, cont, data) ||
             match(node->data.right, text, cont, data);
    case NODE_STAR:
      return match_star(node->data.child, text, cont, data);
    case NODE_PLUS: {
      StarCtx ctx = {node->data.child, text, cont, data};
      return match(node->data.child, text, star_inner_cont, &ctx);
    }
    case NODE_QUESTION:
      if (match(node->data.child, text, cont, data)) return 1;
      return cont(text, data);
    case NODE_BRACES:
      return match_braces(node->data.child, node->data.l, node->data.r, 0, text,
                          cont, data);
    case NODE_CHAR_CLASS:
      if (is_in_charset(*text, node->data.cs)) return cont(text + 1, data);
      return 0;
    case NODE_INV_CHAR_CLASS:
      if (!is_in_charset(*text, node->data.cs)) return cont(text + 1, data);
      return 0;
    default:
      return 0;
  }
}

static int final_cont(const char* restrict text, void* restrict data) {
  FinalCtx* ctx = (FinalCtx*)data;
  *ctx->end = text;
  return 1;
}

static int is_anchored(ASTNode node[static 1]) {
  switch (node->type) {
    case NODE_CARET:
      return 1;
    case NODE_CONCAT:
      return is_anchored(node->data.left);
    case NODE_GROUP:
      return is_anchored(node->data.child);
    default:
      return 0;
  }
}

const char* regex_search(ASTNode root[restrict static 1],
                         const char* restrict text,
                         const char* match_end[restrict static 1]) {
  FinalCtx ctx = {match_end};
  int anchored = is_anchored(root);

  do {
    if (match(root, text, final_cont, &ctx)) return text;
    if (anchored) break;
  } while (*text++);

  return nullptr;
}