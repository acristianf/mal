#include <cassert>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.hpp"
#include "memory.hpp"
#include "reader.hpp"
#include "types.hpp"

// PROTOTYPES
internal data_type_node_t *read_form(reader_t *reader);

reader_t reader = {};

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
  case '\n':
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

internal reader_t *tokenize(reader_t *reader, const char *str) {
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
      if (*(c + 1)) {
        c++;
        while (isspace(*c))
          c++;
        size_t len = 0;
        const char *start = c;
        for (; *c != '\0' && !issymbol(c); c++, len++)
          ;
        if (len > 0) {
          c--;
          token_t *tok = make_token(TOK_LITERAL, start, len);
          head->next = tok;
          head = tok;
        }
      }
    } break;
    case ':': {
      if (*(c + 1)) {
        c++;
        while (isspace(*c))
          c++;
        size_t len = 0;
        const char *start = c;
        for (; *c != '\0' && !issymbol(c); c++, len++)
          ;
        if (len > 0) {
          c--;
          token_t *tok = make_token(TOK_KEYWORD, start, len);
          head->next = tok;
          head = tok;
        }
      }
    } break;
    case '^': {
      token_t *tok = make_token(TOK_CIRCUMFLEX, c, 1);
      head->next = tok;
      head = tok;
    } break;
    case '`': {
      token_t *tok = make_token(TOK_QUASIQUOTE, c, 1);
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
        printf("Parser error: unbalanced string '\"' missing.\n");
      }
    } break;
    default: {
      if (*c) {
        size_t len = 0;
        const char *start = c;
        for (; *c != '\0' && !issymbol(c); c++, len++)
          ;
        if (len > 0) {
          c--;
          token_t *tok = NULL;
          if (strncmp("false", start, len) == 0) {
            tok = make_token(TOK_FALSE, start, len);
          } else if (strncmp("true", start, len) == 0) {
            tok = make_token(TOK_TRUE, start, len);
          } else if (strncmp("nil", start, len) == 0) {
            tok = make_token(TOK_NIL, start, len);
          } else {
            tok = make_token(TOK_LITERAL, start, len);
          }
          assert(tok);
          head->next = tok;
          head = tok;
        }
      }
    } break;
    }
    if (*c) {
      c++;
    }
  };

  token_t *tok = make_token(TOK_EOF, "\0", 1);
  head->next = tok;
  head = tok;

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
  if (r->current->type == TOK_EOF)
    return r->current;
  token_t *tok = r->current;
  r->current = r->current->next;
  return tok;
}

internal data_type_node_t *read_atom(reader_t *reader) {
  data_type_node_t *data = (data_type_node_t *)malloc(sizeof(data_type_node_t));
  data->next = NULL;
  token_t *tok = next(reader);
  switch (tok->type) {
  case TOK_LITERAL: {
    const char *c = tok->literal;
    // Allow for + or - integer prefixes
    if ((*c == '+' || *c == '-') && *(c + 1)) {
      c++;
    }
    while (isdigit(*c)) {
      c++;
    }
    if (!*c) {
      data->number = atoi(tok->literal);
      data->type = VAL_INTEGER;
    } else {
      data->string = strdup(tok->literal);
      data->type = VAL_SYMBOL;
    }
  } break;
  case TOK_KEYWORD: {
    char buf[KEYWORD_SIZE];
    // FIRST CHARACTER FOR KEYWORDS IS 0xFF
    snprintf(buf, sizeof(buf), "%c", 127);
    strcat(buf, tok->literal);
    data->string = strdup(buf);
    data->type = VAL_STRING;
  } break;
  case TOK_STRING_LITERAL: {
    data->string = strdup(tok->literal);
    data->type = VAL_STRING;
  } break;
    // TODO: Implement quotes and quasiquotes correctly
  case TOK_SINGLE_QUOTE: {
    data->string = "quote";
    data->type = VAL_SYMBOL;
  } break;
  case TOK_QUASIQUOTE: {
    data->string = "quasiquote";
    data->type = VAL_SYMBOL;
  } break;
  case TOK_FALSE: {
    data->string = "false";
    data->type = VAL_FALSE;
  } break;
  case TOK_TRUE: {
    data->string = "true";
    data->type = VAL_TRUE;
  } break;
  case TOK_NIL: {
    data->string = "nil";
    data->type = VAL_NIL;
  } break;
  default: {
    // TODO: IMPLEMENT ERRORS
    free(data);
    return NULL;
  } break;
  }
  return data;
}

internal data_type_node_t *read_list(reader_t *reader) {
  data_type_node_t *list = NULL;
  MEM_ALLOC(list, data_type_node_t);
  list->type = VAL_LIST;
  list->next = NULL; // Always NULL when it's list

  list->lst = NULL;
  MEM_ALLOC(list->lst, data_type_node_t);

  data_type_node_t *head = list->lst;
  head->next = NULL;

  data_type_node_t *saved = NULL;

  token_t *cur_tok = next(reader);
  while ((cur_tok = peek(reader))->type != TOK_EOF &&
         cur_tok->type != TOK_RPAREN) {
    data_type_node_t *data = read_form(reader);
    *head = *data;
    saved = head;
    MEM_ALLOC(head->next, data_type_node_t);
    head = head->next;
    free(data);
  }

  if (saved) {
    free(saved->next);
    saved->next = NULL;
  } else {
    free(list->lst);
    list->lst = NULL;
  }

  // TODO: handle error
  if (cur_tok->type != TOK_RPAREN) {
    printf("Parser error: Expected ')'. unbalanced pair.\n");
  }
  next(reader);
  return list;
}

internal data_type_node_t *read_vector(reader_t *reader) {
  data_type_node_t *vector = NULL;
  MEM_ALLOC(vector, data_type_node_t);
  vector->type = VAL_VECTOR;
  vector->next = NULL; // Always NULL when it's list

  vector->lst = NULL;
  MEM_ALLOC(vector->lst, data_type_node_t);

  data_type_node_t *head = vector->lst;
  head->next = NULL;

  data_type_node_t *saved = NULL;

  token_t *cur_tok = next(reader);
  while ((cur_tok = peek(reader))->type != TOK_EOF &&
         cur_tok->type != TOK_RBRACE) {
    data_type_node_t *data = read_form(reader);
    *head = *data;
    saved = head;
    MEM_ALLOC(head->next, data_type_node_t);
    head = head->next;
    free(data);
  }

  if (saved) {
    free(saved->next);
    saved->next = NULL;
  } else {
    free(vector->lst);
    vector->lst = NULL;
  }

  // TODO: handle error
  if (cur_tok->type != TOK_RBRACE) {
    printf("Parser error: Expected ']'. unbalanced pair.\n");
  }
  next(reader);
  return vector;
}

internal data_type_node_t *read_hashmap(reader_t *reader) {
  data_type_node_t *hash_map = NULL;
  MEM_ALLOC(hash_map, data_type_node_t);
  hash_map->type = VAL_HASHMAP;
  hash_map->next = NULL; // Always NULL when it's list

  hash_map->lst = NULL;
  MEM_ALLOC(hash_map->lst, data_type_node_t);

  data_type_node_t *head = hash_map->lst;
  head->next = NULL;

  data_type_node_t *saved = NULL;

  token_t *cur_tok = next(reader);
  while ((cur_tok = peek(reader))->type != TOK_EOF &&
         cur_tok->type != TOK_RCURLY) {
    data_type_node_t *data = read_form(reader);
    *head = *data;
    saved = head;
    MEM_ALLOC(head->next, data_type_node_t);
    head = head->next;
    free(data);
  }

  if (saved) {
    free(saved->next);
    saved->next = NULL;
  } else {
    free(hash_map->lst);
    hash_map->lst = NULL;
  }

  // TODO: handle error
  if (cur_tok->type != TOK_RCURLY) {
    printf("Parser error: Expected '}'. unbalanced pair.\n");
  }
  next(reader);
  return hash_map;
}

internal data_type_node_t *read_form(reader_t *reader) {
  data_type_node_t *result = NULL;
  token_t *current = peek(reader);
  switch (current->type) {
  case TOK_LPAREN: {
    result = read_list(reader);
  } break;
  case TOK_LBRACE: {
    result = read_vector(reader);
  } break;
  case TOK_LCURLY: {
    result = read_hashmap(reader);
  } break;
  case TOK_START: {
    next(reader);
    result = read_form(reader);
  } break;
  case TOK_EOF: {
  } break;
  default: {
    result = read_atom(reader);
  } break;
  }
  return result;
}

internal void print_tokens(reader_t *reader) {
  token_t *tok = reader->current;
  while (tok) {
    printf("TOKEN: ");
    switch (tok->type) {
    case TOK_FALSE:
      printf("TOK_FALSE");
      break;
    case TOK_TRUE:
      printf("TOK_TRUE");
      break;
    case TOK_NIL:
      printf("TOK_NIL");
      break;
    case TOK_COMMENT:
      printf("TOK_COMMENT");
      break;
    case TOK_EOF:
      printf("TOK_EOF");
      break;
    case TOK_LITERAL:
      printf("LITERAL TOKEN");
      break;
    case TOK_KEYWORD:
      printf("KEYWORD TOKEN");
      break;
    default:
      printf("SYMBOL TOKEN");
      break;
    }
    printf(" [%s]\n", tok->literal);
    tok = tok->next;
  }
}

internal void reader_free(reader_t *reader) {
  token_t *tok = reader->token_list;
  while (tok) {
    free(tok->literal);
    token_t *tmp = tok;
    tok = tok->next;
    free(tmp);
  }
}

data_type_node_t *read_str(const char *str) {
  tokenize(&reader, str);
  // print_tokens(&reader);
  data_type_node_t *ast = read_form(&reader);
  reader_free(&reader);
  return ast;
}
