#pragma once

#include <stddef.h>

#include "types.hpp"

#define t hash_table_t

typedef struct bucket_t {
  const char *key;
  data_type_node_t data;
  struct bucket_t *link;
} bucket_t;

typedef struct t {
  size_t count;
  size_t capacity;
  bucket_t buckets[];
} t;

t *hash_table_create(); 

void hash_table_set(t *ht, const char *key, data_type_node_t data);
data_type_node_t hash_table_get(t *ht, const char *key);

#undef t
