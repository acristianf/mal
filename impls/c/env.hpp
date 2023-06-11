#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "types.hpp"
#include "hash_table.hpp"
#include "common.hpp"

/*
 * env = {
 *   symbol: function(...)
 * }
 * */

typedef struct env_t env_t;
typedef struct env_t {
  hash_table_t *symbol_table; // Maps symbols to numeric functions
} env_t;

inline void env_create(env_t *env) {
  env->symbol_table = hash_table_create();
  hash_table_insert(env->symbol_table, "+", add);
  hash_table_insert(env->symbol_table, "-", substract);
  hash_table_insert(env->symbol_table, "*", multiply);
  hash_table_insert(env->symbol_table, "/", divide);
};

inline void env_destroy(env_t *env) {
  free(env->symbol_table);
}
