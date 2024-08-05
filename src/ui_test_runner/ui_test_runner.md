# Implementation: ui_test_runner

An executable for running the tests.

## [Index](../../README.md)
- [Implementation](../README.md)
  - [Test Runner](./README.md)
    - **[ui_test_runner](./ui_test_runner.md)**

### ui_test_runner

The libraries need to be linked to `ui_test_runner` with the `--whole-archive` option in order for the
test constructors to be found by the executable. The `ui_test_runner` CMake function can be used to simplify this task.

Before any targets with tests are created, call `ui_test_runner` with the `WHOLE_ARCHIVE` option. This will enable
the `--whole-archive` linker option.

```cmake
ui_test_runner (WHOLE_ARCHIVE)
```

Then link the library to the test runner with the `LINK_LIBRARY` option. In addition to linking to the runner
target, this will also add a linker option to enable `--whole-archive` for the library.

```cmake
ui_test_runner (LINK_LIBRARY "${TARGET}")
```

After all targets with tests have been linked, call the function one last time with the `NO_WHOLE_ARCHIVE` option.
Failure to do this will result in redefining symbols from common libraries, like the C standard library.

```cmake
ui_test_runner (NO_WHOLE_ARCHIVE)
```
