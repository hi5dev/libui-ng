# Test Framework API

LibUI test framework and runner API.

## [Index](../../README.md)
- [APIs](../README.md)
  - **[Test Framework API](./README.md)**
    - [Synopsis](#synopsis)
    - [Tests are Functions](#tests-are-functions)
    - [Tests are Structures](#tests-are-structures)
    - [A Complete Example](#a-complete-example)
      - [`include/ui/ui_widget.h`](#includeuiui_widgeth)
      - [`src/ui/ui_widget.c`](#srcuiui_widgetc)
      - [`src/ui/ui_widget_test.c`](#srcuiui_widget_testc)
      - [`include/ui/CMakeLists.txt`](#includeuicmakeliststxt)
      - [`src/ui/CMakeLists.txt`](#srcuicmakeliststxt)
    - **API**
      - [ui_test](./ui_test.md)
      - [ui_test_backtrace](./ui_test_backtrace.md)
      - [ui_test_expect](./ui_test_expect.md)
      - [ui_test_main](./ui_test_main.md)
      - [ui_test_options](./ui_test_options.md)
      - [ui_test_report](./ui_test_report.md)
      - [ui_test_report_event](./ui_test_report_event.md)
      - [ui_test_status](./ui_test_status.md)

### Synopsis

Most test frameworks for C use a lot of macros to write functions for you. This can make it very difficult to debug.
This framework only uses macros that can cause errors at compile time when used incorrectly. The main goal here is to
have a framework that doesn't obfuscate the code too much, and to avoid creating a domain-specific-language. You
should still feel like you're programming in C when you're writing your tests.

It should also be very easy to add tests, and you should be able to keep them side-by-side with the code that's
being tested. For example, if you have a file named `ui_feature.c`, you should be able to write your tests directly
inside of that file if you so choose, or include your tests in a file named `ui_feature_test.c` in the same folder.

### Tests are Functions

Here's what I consider a BAD design:

```c++
TEST (suite_name, test_name)
{
  assert (something);
}
```

On the surface, it looks quite nice. That is, until you expand the `TEST` macro, only to get a complete mess of
functions and code that is very hard on the eyes, and difficult for any mere mortal to understand. Unfortunately, a
lot of times the horrible, macro-generated test functions are what you end up seeing in the debugger when you run
into problems.

This framework solves that problem by using normal functions for your tests, like this:

```c++
static ui_test_case ui_test_example (void);
```

The only macro used here, `ui_test_case`, expands to an `__attribute__` or `__declspec`, depending on the compiler
you're using, that registers the function to run automatically. It also includes `void` so you don't write functions
with unexpected return types for the tests.

Here's what the above line of code expands to for GNU C:

```c++
static void __attribute__((constructor)) ui_test_example (void);
```

The intention here is pretty straightforward: The function is registered to run at program startup, sometime before
`main` is called.

### Tests are Structures

Of course, tt's not ideal to have the tests themselves run before `main`. So, the function actually runs twice:
Once at the test application's startup to register the test with the framework, and again later by `main` to
actually run the test.

To make it possible to write your tests in the same function that registers them, one more macro is used. This one
creates a static variable to hold the test's state, and exits the function after registering it:

```c++
static struct ui_test_t test = ui_test (test, ui_test_example);
```

The macro used here is `ui_test`, and the above line of code expands to this:

```c++
static struct ui_test_t test = { UI_TEST_STATUS_REGISTER, { 0, __FILE__, __LINE__ }, 0 };

if (ui_test_register (&test))
  return;
```

Here's how it works:

- When the function is called at startup, it instantiates the static storage for the test's state with an initial
  status of `UI_TEST_STATUS_REGISTER`, some backtrace information, and zeros everything else out.
- The test is then registered by `ui_test_register`. Its state is updated to `UI_TEST_STATUS_PENDING`, and the
  function returns.
- The next time the function runs, `ui_test_register` returns false, so the function continues, and your tests run.

It's worth noting that `ui_test_register` doesn't only check for `UI_TEST_STATUS_PENDING`. The function will
actually only continue if the test's status is set to `UI_TEST_STATUS_RUNNING`. This is handled by the test
framework when you run the test with a function like `ui_test_run_one`.

Here's a complete example of a test without any expanded macros:

```c++
#include <ui_test.h>
#include <ui_test_expect.h>

static ui_test_case
ui_test_example (void)
{
  static struct ui_test test = ui_test (test, ui_test_example);

  ui_expect (1 == 1, "1 != 1??");
}
```

All of this can be easily rewritten without any macros, and the expanded code is very easy to understand, even for
us mere mortals. Sure, it could be simplified even further with one or two more macros, but any benefit you get
from doing that is quickly outweighed by the complexities introduced while debugging.

### A Complete Example

This example demonstrates how to add a new component to this library named `ui_widget`. It doesn't do anything.
This code is very minimal, and is just to demonstrate how to get started with adding a new feature.

#### `include/ui/ui_widget.h`

This is the widget's platform-independent API. For this demonstration, I'll just spec out a structure for the
widget, with a constructor and destructor that we can test later.

```c++
#pragma once

struct ui_widget_t;

struct ui_widget_t *ui_widget_create (void);

void ui_widget_destroy (struct ui_widget_t *ui_widget);
```

#### `src/ui/ui_widget.c`

Here's the platform-independent implementation that uses malloc to keep things simple.

```c++
#include <ui_widget.h>

#include <malloc.h>

struct ui_widget_t
{
  // This is just to satisfy the non-empty struct requirement some C compilers have.
  int unused;
};

struct ui_widget_t *
ui_widget_create (void)
{
  return calloc (1, sizeof (struct ui_widget_t));
}

void
ui_widget_destroy (struct ui_widget_t *ui_widget)
{
  free (ui_widget);
}
```

#### `src/ui/ui_widget_test.c`

And here's our test to ensure the widget can be constructed.

```c++
#include <ui_test.h>
#include <ui_test_expect.h>

#include <ui_widget.h>

#include <stddef.h>

static ui_test_case
ui_widget_test_ctor (void)
{
  static struct ui_test_t test = ui_test (test, ui_widget_test_ctor);

  struct ui_widget_t *widget = ui_widget_create ();
  ui_expect (widget != NULL, "widget was not constructed");
  ui_widget_destroy (widget);
}
```

#### `include/ui/CMakeLists.txt`

You'll need to add the header to CMake. If you were actually adding this to the library, you'd want to simply
append `ui_widget.h` to the existing list instead of calling `target_sources` yourself.

```cmake
target_sources (ui_interface INTERFACE FILE_SET HEADERS FILES ui_widget.h)
```

#### `src/ui/CMakeLists.txt`

Similarly, we can add the source to the implementation library, and then conditionally add the test, depending on
whether or not the test target exists. Again, you'd want to simply add the sources to the existing calls to
`target_sources` when you're actually working on the library.

```cmake
target_sources (ui PRIVATE ui_widget.c)

if (TARGET ui_test)
  target_sources (ui_test PRIVATE ui_widget_test.c)

  # Every test has its own call to `add_test` so you can run them independently. We
  # don't use macros for this,  because we want to be able to run these from an IDE
  # For example, CLion adds a button next  to all these lines that you can click to
  # easily  run and debug each  test on  its own. If you were to put this  inside a
  # macro, the button would run all the tests created by the macro, adding a couple
  # more irritating steps to picking out just the test you want to run.
  add_test (NAME "ui_widget_test_ctor" COMMAND ui::test -n ui_widget_test_ctor)
endif ()
```
