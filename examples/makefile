all: array mandelbrot

array: array.c ../microcompute.h
	gcc -g array.c -I include/ -I ../ -o array

mandelbrot: mandelbrot.c ../microcompute.h
	gcc -g mandelbrot.c -I include/ -I ../ -o mandelbrot

clean:
	rm -f mandelbrot mandelbrot.bmp