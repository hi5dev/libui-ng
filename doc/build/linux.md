# Building on Linux

This guide outlines how to build this library on Linux.

## [Index](../../README.md)

- [doc](../README.md)
  - [build](./README.md)
  - **[Linux](./linux.md)**
    - [Compose](#compose)
  - [Cross-Compiling](./cross_compiling.md)

### Linux

This project includes a [Dockerfile] for compiling a Debian build of this library. It also has a [Compose] script
that will launch the acceptance and usability tests from Docker on Windows using [WSL] by forwarding X11 to Windows.
This also works on Linux if you want to build and debug your application in an isolated environment, and need to
access the GUI from the host.

#### Compose

Using the included Compose script is the recommended approach when building with Docker. Only build the `linux`
service when building on Linux:

```shell
docker compose build linux
```

You may need to enable X11 forwarding from Docker if you want to run the tests. You can do this with `xhost`:

```shell
xhost +local:docker
```

The Compose script mounts the project's root directory to `/home/libui` on the host. This will configure the
project for a Debug build to `./tmp/debug/build`, and a staged installation to `./tmp/debug/install`:

```shell
docker compose run linux cmake \
  -G Ninja \
  -D CMAKE_BUILD_TYPE=Debug \
  -D CMAKE_INSTALL_PREFIX=/home/libui/tmp/debug/install \
  -B /home/libui/tmp/debug/build \
  -S /home/libui
```

And to build:

```shell
docker compose run linux cmake --build /home/libui/tmp/debug/build
```


[Compose]: https://docs.docker.com/compose/
[Dockerfile]: https://docs.docker.com/reference/dockerfile/
[WSL]: https://learn.microsoft.com/en-us/windows/wsl/install
