#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "common.hpp"
#include "hash_table.hpp"
#include "types.hpp"

/*
 * env = {
 *   symbol: function(...)
 * }
 * */

typedef struct env_t env_t;
typedef struct env_t {
  env_t *outer;
  hash_table_t *symbol_table; // Maps symbols to values(can be functions)
} env_t;

void env_create(env_t *in, env_t *outer);
void env_destroy(env_t *env);

void env_set(env_t *env, const char *symbol, data_type_node_t value);
data_type_node_t env_get(env_t *env, const char *key);
