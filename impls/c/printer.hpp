#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "common.hpp"
#include "types.hpp"

internal char t[1024] = {0};
internal size_t idx = 0;

internal inline bool is_keyword(const char *str) { return *str == 127; }

internal inline void create_string(data_type_node_t *data) {
  if (!data)
    return;
  switch (data->type) {
  case VAL_LIST: {
    t[idx++] = '(';
    create_string(data->lst);
    if (t[idx - 1] == ' ')
      t[idx - 1] = ')';
    else
      t[idx++] = ')';
  } break;
  case VAL_VECTOR: {
    t[idx++] = '[';
    create_string(data->lst);
    if (t[idx - 1] == ' ')
      t[idx - 1] = ']';
    else
      t[idx++] = ']';
  } break;
  case VAL_HASHMAP: {
    t[idx++] = '{';
    create_string(data->lst);
    if (t[idx - 1] == ' ')
      t[idx - 1] = '}';
    else
      t[idx++] = '}';
  } break;
  case VAL_FALSE:
  case VAL_TRUE:
  case VAL_NIL:
  case VAL_SYMBOL: {
    strcpy(t + idx, data->string);
    idx += strlen(data->string);
    t[idx++] = ' ';
    create_string(data->next);
  } break;
  case VAL_INTEGER: {
    char buf[43];
    snprintf(buf, sizeof(buf), "%lld", data->number);
    strcpy(t + idx, buf);
    idx += strlen(buf);
    t[idx++] = ' ';
    create_string(data->next);
  } break;
  case VAL_STRING: {
    if (is_keyword(data->string)) {
      char buf[KEYWORD_SIZE];
      snprintf(buf, sizeof(buf), ":%s", data->string + 1);
      strcpy(t + idx, buf);
      idx += strlen(buf);
    } else {
      strcpy(t + idx, data->string);
      idx += strlen(data->string);
    }
    t[idx++] = ' ';
    create_string(data->next);
  } break;
  default: {
  } break;
  }
};

inline char *pt_str(data_type_node_t *data) {
  idx = 0;
  memset(t, 0, sizeof(t));
  create_string(data);
  return t;
}
