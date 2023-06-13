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
    // TODO: Maybe try a better solution for the hash table
    data_type_node_t r = env_get(env, ast->string);
    // TODO: Maybe handle error better
    if (r.type == VAL_NIL)
      fprintf(stderr, "EVAL Error: '%s' not found.", ast->string);
    data_type_node_t *result = NULL;
    MEM_ALLOC(result, data_type_node_t);
    *result = r;
    return result;
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

data_type_node_t *eval_def(data_type_node_t *ast, env_t *env) {
  if (!ast->lst->next || !ast->lst->next->next) {
    fprintf(stderr, "EVAL Error: def! needs a symbol and a value to bind. "
                    "'def! sym val'.\n");
    return ast;
  }
  const char *new_sym = ast->lst->next->string;
  data_type_node_t *bind = EVAL(ast->lst->next->next, env);
  env_set(env, new_sym, *bind);
  return eval_ast(bind, env);
}

data_type_node_t *eval_let(data_type_node_t *ast, env_t *env) {
  env_t let_env;
  env_create(&let_env, env);
  assert(ast->lst->next->type == VAL_LIST ||
         ast->lst->next->type == VAL_VECTOR ||
         ast->lst->next->type == VAL_HASHMAP);
  data_type_node_t *bind = ast->lst->next->lst;
  while (bind) {
    if (!bind->next) {
      fprintf(stderr, "EVAL Error: let* needs a list of symbol and "
                      "values to bind.\n");
      return bind;
    }
    data_type_node_t *r = EVAL(bind->next, &let_env);
    assert(bind->type == VAL_SYMBOL || bind->type == VAL_STRING);
    env_set(&let_env, bind->string, *r);
    bind = bind->next->next;
  }
  return EVAL(ast->lst->next->next, &let_env);
}

data_type_node_t *READ(char *line) { return read_str(line); };

data_type_node_t *EVAL(data_type_node_t *ast, env_t *env) {
  if (!ast) {
    return NULL;
  }
  if (ast->type != VAL_LIST) {
    return eval_ast(ast, env);
  }
  if (ast->type == VAL_LIST) {
    if (!ast->lst) {
      return ast;
    } else if (ast->lst->type == VAL_SYMBOL) {
      if (strcmp(ast->lst->string, "def!") == 0) {
        return eval_def(ast, env);
      } else if (strcmp(ast->lst->string, "let*") == 0) {
        return eval_let(ast, env);
      }
    }
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
  env_create(&repl_env, NULL);
  env_set(&repl_env, "+", func_node_make(add));
  env_set(&repl_env, "-", func_node_make(substract));
  env_set(&repl_env, "*", func_node_make(multiply));
  env_set(&repl_env, "/", func_node_make(divide));

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
