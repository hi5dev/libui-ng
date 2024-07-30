# LibUI Build Instructions

This project is built with [CMake].

## [Index](../../README.md)

- [doc](../README.md)
  - **[build](../build/README.md)**
    - [General](#general)
    - [CMake Options](#cmake-options)
    - [Linux](./linux.md)
    - [OSX](./osx.md)
    - [Win32](./win32.md)
    - [Cross-Compiling](./cross_compiling.md)
  - [examples](../examples/README.md)
  - [linux](../linux/README.md)
  - [osx](../osx/README.md)
  - [schemas](../schemas/README.md)
  - [win32](../win32/README.md)

### General

Here's a typical cmake command for building a Debug release. It builds it using [Ninja] to `./tmp/debug/build`, and
installs it to `./tmp/debug/install`.

```shell
cmake \
  -G Ninja \
  -D CMAKE_BUILD_TYPE=Debug \
  -D CMAKE_INSTALL_PREFIX=./tmp/debug/install \
  -B tmp/dbeug/build \
  -S .
```

### CMake Options

These options can be configured either from the command-line, or from another CMake project. They all default to
`OFF` when including this project as a subproject, and `ON` when it is the top-level project.

| Options             | Description                                                           |
|:--------------------|:----------------------------------------------------------------------|
| `UI_BUILD_EXAMPLES` | Build example programs found in [doc/examples](../examples/README.md) |
| `UI_BUILD_DOCS`     | Build documentation generated from [Doxygen] source-code annotations  |
| `UI_BUILD_TESTS`    | Build the acceptance, unit, and usability test executables.           |
| `UI_INSTALL`        | Generate installation/redistribution packages.                        |

### Platform-Specific Instructions

Here are the guides for building on specific platforms:

- [Linux](./linux.md)
- [Win32](./win32.md)
- [OSX](./osx.md)

There's also a guide for cross-compiling between Linux and Windows:

- [Cross-Compiling](./cross_compiling.md)

[CMake]: https://cmake.org
[Doxygen]: https://www.doxygen.nl/
[Ninja]: https://ninja-build.org/
