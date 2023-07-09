CC       := gcc
GLCC     := glslangValidator -V -S comp

INCLUDES := -I ../
LIBS     := -lvulkan
FLAGS    := -g -O3 -std=c99 -Wall -Wextra -pedantic

.PHONY: all clean

all: array

array.spv: array.glsl
	$(GLCC) array.glsl -o array.spv

array: array.spv array.c ../microcompute.h
	$(CC) $(FLAGS) $(INCLUDES) $@.c -o $@ $(LIBS)

clean:
	rm -f array array.spv