CC       := gcc
GLCC     := glslangValidator -V -S comp

INCLUDES := -I ../
LIBS     := -lvulkan
FLAGS    := -g -O3 -std=c99 -Wall -Wextra -pedantic

.PHONY: all clean

all: array mandelbrot

array.spv: array.glsl
	$(GLCC) array.glsl -o $@

mandelbrot.spv: mandelbrot.glsl
	$(GLCC) mandelbrot.glsl -o $@

array: array.spv array.c ../microcompute.h
	$(CC) $(FLAGS) $(INCLUDES) $@.c -o $@ $(LIBS)

mandelbrot: mandelbrot.spv mandelbrot.c ../microcompute.h
	$(CC) $(FLAGS) $(INCLUDES) $@.c -o $@ $(LIBS)

clean:
	rm -f array array.spv mandelbrot mandelbrot.spv mandelbrot.bmp