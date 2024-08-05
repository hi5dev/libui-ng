# Implementation: Test Runner

An executable for running the tests.

## [Index](../../README.md)
- [Implementation](../README.md)
  - **[Test Runner](./README.md)**
    - [Adding a Test](#adding-a-test)
    - [ui_test_runner](./ui_test_runner.md)

### Adding a Test

Here's a template you can use for including your tests in the test runner.

```cmake
if (TARGET ui::test::runner)
  ui_test_runner (LINK_LIBRARY "ui::target::alias")

  add_test (NAME "ui_target/test/name" COMMAND ui::test::runner "-n ui_target_test_function")
endif ()
```
