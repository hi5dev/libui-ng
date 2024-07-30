### Cross-Compiling

This guide shows you how to cross-compile this project between Linux and Windows.

## [Index](../../README.md)

- [doc](../README.md)
  - [build](./README.md)
  - **[Cross-Compiling](./cross_compiling.md)**
    - [Linux to Windows](#linux-to-windows)
    - [Windows to Linux](#windows-to-linux)

### Linux to Windows

There is a [CMake Toolchain](https://cmake.org/cmake/help/v3.25/manual/cmake-toolchains.7.html) in the project's
root directory ([mingw_toolchain.cmake](../../mingw_toolchain.cmake)) that you can use to compile Windows binaries on
Linux. To use it, set the `CMAKE_TOOLCHAIN_FILE` variable when configuring CMake:

```shell
cmake -G Ninja -D CMAKE_TOOLCHAIN_FILE=mingw_toolchain.cmake # ...
```

You will need to have the 64-bit MinGW binaries installed for this to work. On Ubuntu or Debian, you can install it
by running this as the root user:

```shell
apt install mingw-w64-x86-64-dev
```

### Windows to Linux

Use the included [Compose] to cross-compile from Windows to Linux. Additional information can be found in the
[Linux](./linux.md) build guide.
