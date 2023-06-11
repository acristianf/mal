ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    windows := true
endif

CC=gcc
CFLAGS=-Wextra -Werror -ggdb

build: step0_repl step1_read_print step2_eval

clean:
	rm -f step0_repl step1_read_print step2_eval

step0_repl: step0_repl.cpp reader.cpp editline/linenoise.c
	$(CC) $(CFLAGS) $^ -o $@

step1_read_print: reader.cpp editline/linenoise.c 
	$(CC) $(CFLAGS) $@.cpp $^ -o $@ 

step2_eval: reader.cpp editline/linenoise.c hash_table.cpp
	$(CC) $(CFLAGS) $@.cpp $^ -o $@ 

hash_table:
	$(CC) $(CFLAGS) $@.cpp $^ -o $@ 
