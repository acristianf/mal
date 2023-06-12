#pragma once

#include <malloc.h>
#include <stddef.h>

#include "memory.hpp"

#define KEYWORD_SIZE 128

typedef long long integer_t;

typedef enum types_enum_t {
  VAL_NIL,
  VAL_LIST,
  VAL_VECTOR,
  VAL_HASHMAP,
  VAL_INTEGER,
  VAL_STRING,
  VAL_SYMBOL,
  VAL_TRUE,
  VAL_FALSE,
  VAL_FN,
} types_enum_t;

typedef struct data_type_node_t data_type_node_t;
typedef data_type_node_t *(*fn_t)(data_type_node_t *);

struct data_type_node_t {
  types_enum_t type;
  union {
    integer_t number;
    const char *string;
    data_type_node_t *lst;
    fn_t fun;
  };
  data_type_node_t *next;
};

inline extern data_type_node_t func_node_make(fn_t f) {
  data_type_node_t r;
  r.type = VAL_FN;
  r.fun = f;
  r.next = NULL;
  return r;
}

// TODO: Implement other types
inline extern data_type_node_t *add(data_type_node_t *args_list) {
  data_type_node_t *result = NULL;
  MEM_ALLOC(result, data_type_node_t);
  result->type = args_list->type;
  result->next = NULL;
  switch (result->type) {
  case VAL_INTEGER: {
    long long s = 0;
    while (args_list) {
      s += args_list->number;
      args_list = args_list->next;
    }
    result->number = s;
  } break;
  default:
    result->type = VAL_NIL;
    break;
  }
  return result;
};

// TODO: Implement other types
inline extern data_type_node_t *substract(data_type_node_t *args_list) {
  data_type_node_t *result = NULL;
  MEM_ALLOC(result, data_type_node_t);
  result->type = args_list->type;
  result->next = NULL;
  switch (result->type) {
  case VAL_INTEGER: {
    long long s = args_list->number;
    args_list = args_list->next;
    while (args_list) {
      s -= args_list->number;
      args_list = args_list->next;
    }
    result->number = s;
  } break;
  default:
    result->type = VAL_NIL;
    break;
  }
  return result;
};

// TODO: Implement other types
inline extern data_type_node_t *multiply(data_type_node_t *args_list) {
  data_type_node_t *result = NULL;
  MEM_ALLOC(result, data_type_node_t);
  result->type = args_list->type;
  result->next = NULL;
  switch (result->type) {
  case VAL_INTEGER: {
    long long s = args_list->number;
    args_list = args_list->next;
    while (args_list) {
      s *= args_list->number;
      args_list = args_list->next;
    }
    result->number = s;
  } break;
  default:
    result->type = VAL_NIL;
    break;
  }
  return result;
};

// TODO: Implement other types
inline extern data_type_node_t *divide(data_type_node_t *args_list) {
  data_type_node_t *result = NULL;
  MEM_ALLOC(result, data_type_node_t);
  result->type = args_list->type;
  result->next = NULL;
  switch (result->type) {
  case VAL_INTEGER: {
    long long s = args_list->number;
    args_list = args_list->next;
    while (args_list) {
      assert(args_list->number != 0);
      s /= args_list->number;
      args_list = args_list->next;
    }
    result->number = s;
  } break;
  default:
    result->type = VAL_NIL;
    break;
  }
  return result;
};
