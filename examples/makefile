.PHONY: all test_arrays test_mandelbrot

all: test_arrays test_mandelbrot

test_arrays:
	gcc -I ../out -L ../out arrays.c -o ../out/arrays -lmicrocompute -lEGL
	./../out/arrays

test_mandelbrot:
	gcc -I ../out -L ../out  mandelbrot.c -o ../out/mandelbrot -lmicrocompute -lEGL
	./../out/mandelbrot
