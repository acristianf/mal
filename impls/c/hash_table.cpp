#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_HASH_TABLE_SIZE 101

#include "common.hpp"
#include "hash_table.hpp"
#include "types.hpp"

internal size_t fnv1a_hash_str(const char *in, size_t len) {
  size_t h = 0;
  size_t c;
  for (size_t i = 0; i < len; i++) {
    c = ((const unsigned char *)in)[i];
    c = tolower((int)c); // Ignore case
    h ^= c;
    h *= 16777619;
  }
  return h;
}

hash_table_t *hash_table_create() {
  hash_table_t *ht = (hash_table_t *)malloc(
      sizeof(hash_table_t) + sizeof(bucket_t) * DEFAULT_HASH_TABLE_SIZE);
  ht->count = 0;
  ht->capacity = DEFAULT_HASH_TABLE_SIZE;
  for (size_t i = 0; i < ht->capacity; i++) {
    ht->buckets[i].key = NULL;
    ht->buckets[i].data.type = VAL_NIL;
    ht->buckets[i].data.string = "";
    ht->buckets[i].data.next = NULL;
    ht->buckets[i].link = NULL;
  }
  return ht;
}

void hash_table_set(hash_table_t *ht, const char *key, data_type_node_t data) {
  assert(key);
  if (data.type == VAL_NIL) 
    return;

  size_t hash = fnv1a_hash_str(key, strlen(key));
  size_t idx = hash % ht->capacity;
  bucket_t *bucket = &ht->buckets[idx];
  if (!bucket->key) {
    bucket->key = key;
    bucket->data = data;
    bucket->link = NULL;
    return;
  }

  if (strcmp(bucket->key, key) == 0) {
    bucket->data = data;
    return;
  }

  bucket_t *prev = bucket;
  bucket = bucket->link;
  while (bucket) {
    if (strcmp(bucket->key, key) == 0) {
      bucket->data = data;
      return;
    }
    bucket_t *prev = bucket;
    bucket = bucket->link;
  }
  assert(prev);

  bucket_t *new_b = (bucket_t *)malloc(sizeof(bucket_t));
  new_b->key = key;
  new_b->data = data;
  new_b->link = NULL;
  prev->link = new_b;
};

data_type_node_t hash_table_get(hash_table_t *ht, const char *key) {
  assert(ht);
  assert(key);
  data_type_node_t f = {.type = VAL_NIL, .string = "", .next = NULL};

  size_t hash = fnv1a_hash_str(key, strlen(key));
  size_t idx = hash % ht->capacity;

  bucket_t *b = &ht->buckets[idx];
  if (!b->key)
    return f;

  if (strcmp(b->key, key) == 0) {
    f = ht->buckets[idx].data;
  } else {
    while (b) {
      if (strcmp(b->key, key) == 0) {
        f = b->data;
        break;
      }
      b = b->link;
    }
  }

  return f;
};

// #define t hash_table_t
// int main() {
//   t *table = hash_table_create();
//   hash_table_insert(table, "+", add);
//   return 0;
// }
