CC       := gcc -g -O3 -std=c99 -Wall -Wextra -pedantic -Wno-unused-parameter
INCLUDES := -I include/ -I ../../

.PHONY: all clean

all: array mandelbrot

array: array.c ../../microcompute.h
	$(CC) $(INCLUDES) $@.c -o $@

mandelbrot: array.c ../../microcompute.h
	$(CC) $(INCLUDES) $@.c -o $@

clean:
	rm -f array mandelbrot mandelbrot.bmp