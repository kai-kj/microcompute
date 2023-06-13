<p align="center">
	<h1 align="center"><b>microcompute</b></h1>
	<p align="center"><b>A small library for gpu computing</b></p>
	<p align="center">
	<img src="https://img.shields.io/github/license/kal39/microcompute">
	<img src="https://img.shields.io/github/repo-size/kal39/microcompute">
	<img src="https://img.shields.io/github/languages/top/kal39/microcompute">
	<br>
	<img src="https://img.shields.io/github/issues/kal39/microcompute">
	<img src="https://img.shields.io/github/v/release/kal39/microcompute?sort=semver">
	</p>
</p>

## About

This library makes it easy to run simple OpenGL compute shaders without creating a window. This is done by using EGL, allowing headless programs to use the GPU.

## Examples

- [Mandelbrot renderer](https://github.com/kal39/microcompute/blob/master/examples/mandelbrot.c)

## Documentation

See [`doc.md`](https://github.com/kal39/microcompute/blob/master/doc.md), this was generated using [`mini_doc_gen.py`](https://kaikitagawajones.com/projects_mini_doc_gen.html).

## Building

To clone the repository, run:

```
git clone git@github.com:kal39/microcompute.git
cd microcompute
```

To build the library run:

```
mkdir build
cd build
cmake ..
make
```

The header files can be found in `include/`, and the library file can be found in `build/`.

To build the examples run:

```
cd examples
mkdir build
cmake ..
make
```

The binaries can be found in `examples/build`.

## License

MIT (see [LICENSE](https://github.com/kal39/microcompute/blob/master/LICENSE)).
