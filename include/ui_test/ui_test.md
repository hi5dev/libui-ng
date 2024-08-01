# Test Framework API: ui_test

Structure for a test's current state, pointers to its callbacks, backtrace information, and for registering tests.
Macros for creating and registering tests, and functions used internally by [ui_test_main](./ui_test_main.md).

## [Index](../../README.md)
- [APIs](../README.md)
  - [Test Framework API](./README.md)
    - **[ui_test](./ui_test.md)**
      - [Macros](#macros)
        - [ui_test_runner](#ui_test_runner)
        - [ui_test](#ui_test)
      - [Types](#types)
        - [ui_test_cb_t](#ui_test_cb_t)
        - [ui_test_t](#ui_test_t)
      - [Functions](#functions)
        - [ui_test_run_all](#ui_test_run_all)
        - [ui_test_run_by_name](#ui_test_run_by_name)
        - [ui_test_run_one](#ui_test_run_one)
        - [ui_test_set_status](#ui_test_set_status)
        - [ui_test_register](#ui_test_register)

### Macros

#### `ui_test_runner`

Registers a function to run at the application's startup for registering a test before `main` is called.

```c++
static void ui_test_runner ui_test_example (void);
```

#### `ui_test`

Creates a new `ui_test_t` instance and registers a test with the framework. Includes a guard to ensure the tests
don't run during the test-registration phase.

```c++
static struct ui_test_t test = ui_test (ui_test_example);
```

### Types

#### `ui_test_cb_t`

Callback function-type used by `ui_test_t`.

```c++
static void ui_test_cb_example (void);
```

#### `ui_test_t`

Contains stateful information for a test. Includes its current status, backtrace information (see
[u_test_backtrace](./ui_test_backtrace.md)), and options typically assigned by the user at the command-line. Also
serves as a double-linked list that's used for test registration.

```c++
static struct ui_test_t test = {};
```

##### `.status`

See [ui_test_status](./ui_test_status.md).

```c++
test.status = UI_TEST_STATUS_FAILED;
```

##### `.backtrace`

See [ui_test_backtrace](./ui_test_backtrace.md).

```c++
test.backtrace.message = "test expectation failed";
```

##### `.name`

Identifies the test to the user, and is typically the same as test function's name.

```c++
test.name = "ui_test_example";
```

##### `.run`

Points to the function that is called to both register and run the test.

```c++
test.run = ui_test_example;
```

##### `.options`

See [ui_test_options](./ui_test_options.md).

```c++
test.options.verbose = 1;
```

##### `.report`

Points to the current [test report](./ui_test_report.md). This is typically only used by test reporters, and the
test runner.

```c++
ui_test_set_status (&test, UI_TEST_STATUS_RUNNING, "hello", __FILE__, __LINE__);
ui_test_report_dispatch (test.report, UI_TEST_REPORT_EVENT_BACKTRACE);
```

##### `.previous`

Part of a double-linked list used to keep track of and run the tests.

```c++
test.previous = some_static_ui_test_t;
```

##### `.next`

Part of a double-linked list used to keep track of and run the tests.

```c++
test.next = some_static_ui_test_t;
```

### Functions

Outside of maintaining this library, you'll likely never use any of these functions. Instead, you'll use the macros
or other functions that call them. They're mainly used internally, and subject to change in a way that could break
your code if you use them directly.

#### `ui_test_register`

Registers a test if it hasn't already been, and returns `0` when the caller can continue by running the tests. Any
non-zero value indicates the test isn't ready to run. See the Doxygen comments in the code for more information
about other possible return values.

```c++
const int safe_to_run = ui_test_register(&test);
```

#### `ui_test_run_all`

Runs all currently registered tests.

```c++
ui_test_run_all (&report, &options);
```

#### `ui_test_run_by_name`

Runs all tests whose `name` are an exact match to the given string. This doesn't stop when finding a match so you
can give multiple tests functions the same name for grouping them together.

```c++
ui_test_run_by_name ("ui_example_test", &report, &options);
```

#### `ui_test_run_one`

Runs only one specific test.

```c++
ui_test_run_one (&test, &report, &options);
```

#### `ui_test_set_status`

Mainly used by [ui_test_expect](./ui_test_expect.md) to update a test's status and [backtrace](./ui_test_backtrace.md).

```c++
ui_test_set_status (&test, UI_TEST_STATUS_SKIPPED, "work-in-progress", __FILE__, __LINE__);
```
