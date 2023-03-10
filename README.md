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

This library makes it easy to run simple OpenGL compute shaders. It can be used in two ways:

- As a standalone library  
  When used in this way, it is possible to run OpenGL compute shaders without creating a window. This is done by using [Linux DRI](https://en.wikipedia.org/wiki/Direct_Rendering_Infrastructure), allowing headless programs to utilise the GPU.
- Inside a program that already uses OpenGL  
  An example is the [microcanvas](https://github.com/kal39/microcanvas) library I am also working on.

## Examples

- [Array operations](https://github.com/kal39/microcompute/blob/master/examples/arrays.c)
- [Mandelbrot renderer](https://github.com/kal39/microcompute/blob/master/examples/mandelbrot.c)

## Documentation

See [doc.md](https://github.com/kal39/microcompute/blob/master/doc.md)

## Building

To clone the repository, run:

```
git clone git@github.com:kal39/microcompute.git
cd microcompute
git submodule update --init --recursive
```

### Standalone version

To compile the microcompute library, run:

```
make library STANDALONE=true
```

A static library file and header file will be generated in `out/`.

The examples can be compiled and run by:

```
make run_examples
```

The compiled example binaries will be generated in `example/`.

When using microcompute in this way, the following libraries are needed:

```
-lmicrocompute -lEGL -lGL
```

### For use inside a OpenGL program

To compile the microcompute library, run:

```
make library
```

When using microcompute in this way, the following libraries are needed:

```
-lmicrocompute -lGL
```

## License

MIT (see [LICENSE](https://github.com/kal39/microcompute/blob/master/LICENSE)).
