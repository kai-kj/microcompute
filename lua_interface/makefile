CC       := gcc -g -O3 -std=c99 -Wall -Wextra -Wno-unused-parameter
INCLUDES := -I ../

.PHONY: all clean

all: microcompute_lua

microcompute_lua: microcompute_lua.c ../microcompute.h
	$(CC) -shared -fPIC $(INCLUDES) $@.c -o $@.so

clean:
	rm -f microcompute_lua.so