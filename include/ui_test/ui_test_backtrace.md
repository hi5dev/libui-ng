# Test Framework API: ui_test_backtrace

Backtrace information mainly used to debug failed tests.

## [Index](../../README.md)
- [APIs](../README.md)
  - [Test Framework API](./README.md)
    - **[ui_test_backtrace](./ui_test_backtrace.md)**
      - [Macros](#macros)
        - [ui_test_backtrace](#ui_test_backtrace)
      - [Types](#types)
        - [ui_test_backtrace_t](#ui_test_backtrace_t)

### Macros

#### `ui_test_backtrace`

Used to statically initialize a `ui_test_backtrace_t` structure with a given message, setting the filename and line
to that of the caller.

```c++
static struct ui_test_backtrace_t backtrace = ui_test_backtrace ("message");
```

### Types

#### `ui_test_backtrace_t`

##### `.message`

User-defined message with information about whatever event triggered an update to the a test's backtrace.

```c++
backtrace.message = "error";
```

##### `.filename`

Full path to the souce of the event.

```c++
backtrace.filename = __FILE__;
```

##### `.line`

Line number inside `filename` that contains the source of the event.

```c++
backtrace.line = __LINE__;
```
