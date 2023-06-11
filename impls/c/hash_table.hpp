#pragma once

#include <stddef.h>

#include "types.hpp"

#define t hash_table_t

typedef struct bucket_t {
  const char *key;
  fn_t func;
  struct bucket_t *link;
} bucket_t;

typedef struct t {
  size_t count;
  size_t capacity;
  bucket_t buckets[];
} t;

t *hash_table_create(); 

void hash_table_insert(t *ht, const char *key, fn_t func);
fn_t hash_table_get(t *ht, const char *key);

#undef t
