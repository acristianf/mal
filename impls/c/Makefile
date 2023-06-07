ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    windows := true
endif

CFLAGS=-Wextra -Werror
SRC=reader.cpp
EXTERNAL_SRC=editline/linenoise.c

step0_repl: 
ifdef $(windows)
	clang $(CFLAGS) step0_repl.cpp $(EXTERNAL_SRC) -o step0_repl.exe 
else 
	clang $(CFLAGS) step0_repl.cpp $(EXTERNAL_SRC) -o step0_repl
endif

step1_read_print: 
ifdef $(windows)
	clang $(CFLAGS) step1_read_print.cpp $(SRC) $(EXTERNAL_SRC) -o step1_read_print.exe 
else 
	clang $(CFLAGS) step1_read_print.cpp $(SRC) $(EXTERNAL_SRC) -o step1_read_print
endif
