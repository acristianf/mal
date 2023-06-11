#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editline/linenoise.h"

#include "env.hpp"
#include "memory.hpp"
#include "printer.hpp"
#include "reader.hpp"
#include "types.hpp"

#define INPUT_LINE "user> "
#define LINE_MAX_INPUT 2048

data_type_node_t *EVAL(data_type_node_t *ast, env_t *env);
data_type_node_t *eval_ast(data_type_node_t *ast, env_t *env);

data_type_node_t *eval_hashmap(data_type_node_t *hm, env_t *env) {
  if (!hm->lst)
    return hm;
  data_type_node_t *data = hm->lst;

  data_type_node_t *evalhm = NULL;
  MEM_ALLOC(evalhm, data_type_node_t);
  evalhm->type = VAL_HASHMAP;

  evalhm->lst = EVAL(data, env);

  data_type_node_t *head = evalhm->lst;
  data = data->next;
  while (data) {
    data_type_node_t *l = EVAL(data, env);
    head->next = l;
    head = head->next;
    data = data->next;
  }
  head->next = NULL;

  return evalhm;
}

data_type_node_t *eval_vector(data_type_node_t *vec, env_t *env) {
  if (!vec->lst)
    return vec;
  data_type_node_t *data = vec->lst;

  data_type_node_t *evalvec = NULL;
  MEM_ALLOC(evalvec, data_type_node_t);
  evalvec->type = VAL_VECTOR;

  evalvec->lst = EVAL(data, env);

  data_type_node_t *head = evalvec->lst;
  data = data->next;
  while (data) {
    data_type_node_t *l = EVAL(data, env);
    head->next = l;
    head = head->next;
    data = data->next;
  }
  head->next = NULL;

  return evalvec;
}

data_type_node_t *eval_list(data_type_node_t *l, env_t *env) {
  data_type_node_t *data = l->lst;

  data_type_node_t *evalst = NULL;
  evalst = EVAL(data, env);
  evalst->next = NULL;

  data_type_node_t *head = evalst;
  data = data->next;
  while (data) {
    data_type_node_t *l = EVAL(data, env);
    head->next = l;
    head = head->next;
    data = data->next;
  }
  head->next = NULL;

  return evalst;
}

data_type_node_t *eval_ast(data_type_node_t *ast, env_t *env) {
  switch (ast->type) {
  case VAL_SYMBOL: {
    fn_t fun = hash_table_get(env->symbol_table, ast->string);
    if (fun) {
      data_type_node_t *f =
          (data_type_node_t *)malloc(sizeof(data_type_node_t));
      f->type = VAL_FN;
      f->fun = fun;
      return f;
    } else {
      data_type_node_t *e = NULL;
      MEM_ALLOC(e, data_type_node_t);
      e->type = VAL_NIL;
      e->string = "";
      e->next = NULL;
      return e;
    }
  } break;
  case VAL_LIST: {
    return eval_list(ast, env);
  } break;
  case VAL_VECTOR: {
    return eval_vector(ast, env);
  } break;
  case VAL_HASHMAP: {
    return eval_hashmap(ast, env);
  } break;
  default: {
    return ast;
  } break;
  }
  return ast;
}

data_type_node_t *READ(char *line) { return read_str(line); };

data_type_node_t *EVAL(data_type_node_t *ast, env_t *env) {
  if (ast->type != VAL_LIST) {
    return eval_ast(ast, env);
  }
  if (ast->type == VAL_LIST && !ast->lst) {
    return ast;
  }

  data_type_node_t *evalst = eval_ast(ast, env);
  if (evalst->type == VAL_NIL) {
    // TODO: Maybe handle known error
    return evalst;
  } else if (evalst->type != VAL_FN) {
    return evalst;
  }
  fn_t f = evalst->fun;

  return f(evalst->next);
}

void PRINT(data_type_node_t *ast) { printf("%s\n", pt_str(ast)); }

void rep(char *line, env_t *env) { PRINT(EVAL(READ(line), env)); }

int main() {

  linenoiseHistoryLoad("history.txt");

  char *line;
  env_t repl_env;
  env_create(&repl_env);
  while (1) {
    line = linenoise(INPUT_LINE);
    if (!line) {
      break;
    }
    rep(line, &repl_env);
    linenoiseHistoryAdd(line);
    linenoiseHistorySave("history.txt");
    free(line);
  }
  return 0;
}
