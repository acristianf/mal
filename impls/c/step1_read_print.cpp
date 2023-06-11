#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editline/linenoise.h"

#include "printer.hpp"
#include "reader.hpp"
#include "types.hpp"

#define INPUT_LINE "user> "
#define LINE_MAX_INPUT 2048

data_type_node_t *READ(char *line) { return read_str(line); };

data_type_node_t *EVAL(data_type_node_t *ast) { return ast; }

void PRINT(data_type_node_t *ast) { printf("%s\n", pt_str(ast)); }

void rep(char *line) { PRINT(EVAL(READ(line))); }

int main() {

  linenoiseHistoryLoad("history.txt");

  char *line;
  while (1) {
    line = linenoise(INPUT_LINE);
    if (!line) {
      break;
    }
    rep(line);
    linenoiseHistoryAdd(line);
    linenoiseHistorySave("history.txt");
    free(line);
  }
  return 0;
}
