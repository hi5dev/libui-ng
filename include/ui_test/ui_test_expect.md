# Test Framework API: ui_test_expect

Test expectation macros and methods.

## [Index](../../README.md)

- [APIs](../README.md)
  - [Test Framework API](./README.md)
    - **[ui_test_expect](./ui_test_expect.md)**
      - [Macros](#macros)
        - [ui_expect](#ui_expect)
        - [ui_expect_cmp](#ui_expect_cmp)
        - [ui_expect_not_null](#ui_expect_not_null)
        - [ui_expect_null](#ui_expect_null)
        - [ui_test_pass](#ui_test_pass)
        - [ui_test_skip](#ui_test_skip)
        - [Internal](#internal)
          - [_ui_expect](#_ui_expect)
          - [ui_test_interrupt](#ui_test_interrupt)

### Macros

#### `ui_expect`

Tests if the given condition evaluates to true. If it doesn't, the test fails with the given message.

```c++
ui_expect (condition, "message");
```

#### `ui_expect_cmp`

Compares two values of a given type for equality.

```c++
ui_expect (int, expected, is, actual);
ui_expect (int, expected, is_not, actual);
```

Supported types:

| Type     | ui_expect parameter | Example                                           |
|:---------|:--------------------|:--------------------------------------------------|
| `int`    | `int`               | `ui_expect_cmp (int, 1, is, 1);`                  |
| `float`  | `float`             | `ui_expect_cmp (float, 1.0f, is_not, 2.0f);`      |
| `double` | `double`            | `ui_expect_cmp (double, 1.0, is, 1.0);`           |
| `char *` | `str`               | `ui_expect_cmp (str, "apple", is_not, "orange");` |

#### `ui_expect_not_null`

Tests if a pointer is not null.

```c++
ui_expect_not_null (ptr);
```

#### `ui_expect_null`

Tests if a pointer is null.

```c++
ui_expect_null (ptr);
```

#### `ui_test_pass`

Marks the current test as passed and continues as normal (does not interrupt the test). Also clears the
test's backtrace message, and sets its filename and line.

```c++
ui_test_pass ();
```

#### `ui_test_skip`

Updates the current test's status as skipped and sets the backtrace message, filename, and line.

```c++
ui_test_skip ("work in progress");
```

> **Remarks**
> - This is ignored if the option to force skipped tests is enabled.
> - The backtrace is printed as long as the option to silence backtraces is not set.
> - The given message is always printed next to the `[SKIP]` in the test report.

### Internal

These macros are used by other macros, and are not intended to be used by the actual tests.

#### `_ui_expect`

Returns from the caller if the given condition evaluates to false.

```c++
_ui_expect (1);     // no return value - used in void functions
_ui_expect (1, 0);  // returns 0
```

#### `ui_test_interrupt`

Interrupts the current test, updating its status and backtrace message.

```c++
ui_test_interrupt (UI_TEST_STATUS_SKIP, "work in progress");
```
