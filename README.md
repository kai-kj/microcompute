<p align="center">
	<h1 align="center"><b>microcompute</b></h1>
	<p align="center"><b>A small library for gpu computing</b></p>
	<p align="center">
	<img src="https://img.shields.io/github/license/kal39/microcompute">
	<img src="https://img.shields.io/github/repo-size/kal39/microcompute">
	<img src="https://img.shields.io/github/languages/top/kal39/microcompute">
	</p>
</p>

## About

This library makes it easy to run simple OpenGL compute shaders without creating a window. This is done by using EGL, allowing headless programs to use the GPU.

Bindings to lua are also provided in `lua_interface/`. [`luajit`](https://luajit.org/luajit.html) is needed for this library.

## Examples

- Array operations
  - [`array.c`](https://github.com/kal39/microcompute/blob/master/examples/c/array.c)
  - [`array.lua`](https://github.com/kal39/microcompute/blob/master/examples/lua/array.lua)
- Mandelbrot renderer
  - [`mandelbrot.c`](https://github.com/kal39/microcompute/blob/master/examples/c/mandelbrot.c)
  - [`mandelbrot.lua`](https://github.com/kal39/microcompute/blob/master/examples/lua/mandelbrot.lua)

## Documentation

- C
  - [`doc.md`](https://github.com/kal39/microcompute/blob/master/doc.md)
  - [`microcompute.h`](https://github.com/kal39/microcompute/blob/master/microcompute.h) (generated using [`mini_doc_gen.py`](https://kaikitagawajones.com/projects_mini_doc_gen.html))
- lua
  - [`microcompute.lua`](https://github.com/kal39/microcompute/blob/master/lua_interface/microcompute.lua)

## Building

- To build the C examples, simply run `make` in `examples/c`.
- The examples can be run with `./array` or `./mandelbrot` (in `examples/c`).
- To build the lua interface, simply run `make` in `microcompute/lua_interface`.
- The lua examples can be run with `luajit array.lua` and `luajit mandelbrot.lua` (in `examples/lua`)

## License

MIT (see [LICENSE](https://github.com/kal39/microcompute/blob/master/LICENSE)).
