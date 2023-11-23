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

A small and simple library to use compute shaders in Vulkan.

## Dependencies

For Ubuntu (tested on `22.04`):

- `build-essential`
- `libvulkan-dev`
- `vulkan-validationlayers-dev` (only for the validation layer)
- `glslang-tools` (only for the included examples)

Or equivalent for other systems.

## Examples

- Array operations
  - [`array.c`](https://github.com/kal39/microcompute/blob/master/examples/array.c)
  - [`array.glsl`](https://github.com/kal39/microcompute/blob/master/examples/array.glsl)
- Mandelbrot renderer
  - [`mandelbrot.c`](https://github.com/kal39/microcompute/blob/master/examples/mandelbrot.glsl)
  - [`mandelbrot.glsl`](https://github.com/kal39/microcompute/blob/master/examples/mandelbrot.glsl)

Run `make all` in `examples/` to build all examples. It requires `gcc` and `glslangValidator` to be installed.

## Documentation

- [`doc.md`](https://github.com/kal39/microcompute/blob/master/doc.md)
- [`microcompute.h`](https://github.com/kal39/microcompute/blob/master/microcompute.h) (generated using [`mini_doc_gen.py`](https://kaikitagawajones.com/projects_mini_doc_gen.html))

## License

MIT (see [LICENSE](https://github.com/kal39/microcompute/blob/master/LICENSE)).
