#pragma once

#include <stddef.h>

#include "types.hpp"

typedef enum token_type_t {
  TOK_ILLEGAL = 0,
  // Literals
  TOK_STRING_LITERAL,
  TOK_LITERAL,
  // Symbols
  TOK_TILDE,
  TOK_AT,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_LBRACE,
  TOK_RBRACE,
  TOK_LCURLY,
  TOK_RCURLY,
  TOK_COMMA,
  TOK_SINGLE_QUOTE,
  TOK_CIRCUMFLEX,
  TOK_QUASIQUOTE,
  // KEYWORDS
  TOK_KEYWORD,
  // RESERVED
  TOK_FALSE,
  TOK_TRUE,
  TOK_NIL,
  // Comments
  TOK_COMMENT,
  // Special
  TOK_TILDE_AT,
  TOK_START,
  TOK_EOF,
} token_type_t;

typedef struct token_t token_t;
struct token_t {
  token_type_t type;
  char *literal; // NULL terminated
  token_t *next;
};

typedef struct reader_t {
  token_t *current;
  token_t *token_list;
} reader_t;
extern reader_t reader;

data_type_node_t *read_str(const char *str);
