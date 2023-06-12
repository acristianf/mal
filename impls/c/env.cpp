#include "env.hpp"
#include "common.hpp"
#include "memory.hpp"

void env_create(env_t *in, env_t *outer) {
  in->outer = outer;
  in->symbol_table = hash_table_create();
};

void env_destroy(env_t *env) { free(env->symbol_table); }

void env_set(env_t *env, const char *symbol, data_type_node_t value) {
  hash_table_set(env->symbol_table, symbol, value);
};

internal data_type_node_t env_find(env_t *env, const char *key) {
  return hash_table_get(env->symbol_table, key);
}

data_type_node_t env_get(env_t *env, const char *key) {
  env_t *cur = env;
  data_type_node_t r;
  r.type = VAL_NIL;
  r.string = "";
  r.next = NULL;
  while (cur) {
    r = env_find(cur, key);
    if (r.type != VAL_NIL)
      return r;
    cur = cur->outer;
  }
  return r;
};
