#include <cassert>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define internal static
#define persist static

#include "reader.hpp"
#include "types.hpp"

// PROTOTYPES
internal data_type_t read_form(reader_t *reader);

internal token_t *make_token(token_type_t type, const char *literal,
                             size_t len) {
  token_t *tok = (token_t *)malloc(sizeof(token_t));
  tok->type = type;
  tok->literal = strndup(literal, len);
  tok->next = NULL;
  return tok;
}

internal bool issymbol(const char *ch) {
  switch (*ch) {
  case ' ':
  case '\t':
  case '~':
  case '@':
  case '[':
  case ']':
  case '(':
  case ')':
  case '{':
  case '}':
  case ';':
  case ':':
  case ',':
  case '\'':
  case '^':
  case '`':
  case '"':
    return true;
  default:
    return false;
  }
}

internal reader_t *tokenize(const char *str) {
  reader_t *reader = (reader_t *)malloc(sizeof(reader_t));
  reader->token_list = make_token(TOK_START, "START", 5);
  reader->current = reader->token_list;

  token_t *head = reader->token_list;
  const char *c = str;
  while (*c != '\0') {
    switch (*c) {
    case '~': {
      if (*(c + 1) && *(c + 1) == '@') {
        token_t *tok = make_token(TOK_TILDE_AT, c, 2);
        head->next = tok;
        head = tok;
        c++;
      } else {
        token_t *tok = make_token(TOK_TILDE, c, 1);
        head->next = tok;
        head = tok;
      }
    } break;
    case '@': {
      token_t *tok = make_token(TOK_AT, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case '\'': {
      token_t *tok = make_token(TOK_SINGLE_QUOTE, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case ':': {
      token_t *tok = make_token(TOK_COLON, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case ',': {
      token_t *tok = make_token(TOK_COMMA, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case '^': {
      token_t *tok = make_token(TOK_CIRCUMFLEX, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case '`': {
      token_t *tok = make_token(TOK_BACKTICK, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case '(': {
      token_t *tok = make_token(TOK_LPAREN, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case ')': {
      token_t *tok = make_token(TOK_RPAREN, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case '[': {
      token_t *tok = make_token(TOK_LBRACE, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case ']': {
      token_t *tok = make_token(TOK_RBRACE, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case '{': {
      token_t *tok = make_token(TOK_LCURLY, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case '}': {
      token_t *tok = make_token(TOK_RCURLY, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case ';': {
      size_t len = 0;
      const char *start = c;
      for (; *c != '\n' && *c != '\0'; c++, len++)
        ;
      token_t *tok = make_token(TOK_COMMENT, start, len);
      head->next = tok;
      head = tok;
    } break;
    case '"': {
      bool found_quote = false;
      size_t len = 1;
      const char *start = c;
      c++;
      for (; *c != '\0' && *c != '\n'; c++, len++) {
        if (*c == '"') {
          found_quote = true;
          c++;
          len++;
          break;
        } else if (*c == '\\') {
          c++;
          len++;
        }
      }
      c--;
      if (found_quote) {
        token_t *tok = make_token(TOK_STRING_LITERAL, start, len);
        head->next = tok;
        head = tok;
      } else {
        // TODO: Handle errors
      }

    } break;
    default: {
      if (*c) {
        size_t len = 0;
        const char *start = c;
        for (; *c != '\0' && *c != '\n'; c++, len++) {
          if (issymbol(c)) {
            c--;
            break;
          }
        }
        token_t *tok = make_token(TOK_LITERAL, start, len);
        head->next = tok;
        head = tok;
      }
    } break;
    }
    if (*c) {
      c++;
    }
  };

  token_t *tok = make_token(TOK_EOF, "\0", 1);
  head->next = tok;

  return reader;
}

internal token_t *peek(reader_t *r) {
  assert(r);
  token_t *tok = r->current;
  return tok;
}

// Returns NULL if at end of list
internal token_t *next(reader_t *r) {
  assert(r);
  if (!r->current)
    return NULL;
  token_t *tok = r->current;
  r->current = r->current->next;
  return tok;
}

internal void reader_free(reader_t *r) {
  assert(r);
  token_t *t = r->token_list;
  while (t) {
    token_t *tmp = t;
    t = t->next;
    free(tmp->literal);
    free(tmp);
  }
  free(r);
}

internal data_type_t read_atom(reader_t *reader) {
  data_type_t data;
  token_t *tok = peek(reader);
  switch (tok->type) {
  case TOK_LITERAL: {
    const char *c = tok->literal;
    while (isdigit(*c)) {
      c++;
    }
    if (!*c) {
      data.number = atoi(tok->literal);
      data.type = VAL_INTEGER;
    }
  } break;
  default: {
    // TODO: IMPLEMENT ERRORS
  } break;
  }
  return data;
}

internal data_type_node_t *read_list(reader_t *reader) {
  data_type_node_t *list = (data_type_node_t *)malloc(sizeof(data_type_node_t));

  data_type_node_t *head = list;
  data_type_t data = read_form(reader);
  head->value = data;
  head->next = NULL;
  token_t *cur_tok = next(reader);
  while (cur_tok->type != TOK_EOF && cur_tok->type != TOK_RPAREN) {
    data_type_t data = read_form(reader);
    data_type_node_t *cur =
        (data_type_node_t *)malloc(sizeof(data_type_node_t));
    cur->value = data;
    cur->next = NULL;
    head->next = cur;
    head = cur;
    cur_tok = next(reader);
  }
  return list;
}

internal data_type_t read_form(reader_t *reader) {
  data_type_t result;
  next(reader);
  token_t *current = peek(reader);
  printf("[%s]\n", current->literal);
  switch (current->type) {
  case TOK_LPAREN: {
    data_type_node_t *list = read_list(reader);
    data_type_node_t *cur = list;
    while (cur) {
      if (cur->value.type == VAL_INTEGER) {
        printf("%llu\n", cur->value.number);
      }
      cur = cur->next;
    }
  } break;
  default: {
    result = read_atom(reader);
    printf("%llu\n", result.number);
  } break;
  }
  return result;
}

void read_str(const char *str) {
  reader_t *reader = tokenize(str);
  read_form(reader);
  reader_free(reader);
}
