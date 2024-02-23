CC       := gcc
GLCC     := glslangValidator -V -S comp

INCLUDES := -I ../
LIBS     := -lvulkan
FLAGS    := -g -O3 -std=c99 -Wall -Wextra -Werror -pedantic -Wno-unused-parameter -D MC_ENABLE_VALIDATION_LAYER

.PHONY: all clean

all: check_devs mandelbrot

check_devs.spv: check_devs.glsl
	$(GLCC) check_devs.glsl -o $@

mandelbrot.spv: mandelbrot.glsl
	$(GLCC) mandelbrot.glsl -o $@

check_devs: check_devs.spv check_devs.c ../microcompute.h
	$(CC) $(FLAGS) $(INCLUDES) $@.c -o $@ $(LIBS)

mandelbrot: mandelbrot.spv mandelbrot.c ../microcompute.h
	$(CC) $(FLAGS) $(INCLUDES) $@.c -o $@ $(LIBS)

clean:
	rm -f check_devs check_devs.spv mandelbrot mandelbrot.spv mandelbrot.bmp