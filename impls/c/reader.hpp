#pragma once

#include <stddef.h>

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
  TOK_COLON,
  TOK_COMMA,
  TOK_SINGLE_QUOTE,
  TOK_CIRCUMFLEX,
  TOK_BACKTICK,
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

void read_str(const char *str);
