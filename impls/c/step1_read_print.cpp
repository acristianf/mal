#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "editline/linenoise.h"

#include "reader.hpp"

#define INPUT_LINE "user> "
#define LINE_MAX_INPUT 2048

char *READ(char *line) {
  read_str(line);
  return line;
};

char *EVAL(char *line) { return line; }

char *PRINT(char *line) { return line; }

char *rep(char *line) { return PRINT(EVAL(READ(line))); }

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
