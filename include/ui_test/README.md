# Test Framework API

LibUI test framework and runner API.

## [Index](../../README.md)
- [APIs](../README.md)
  - **[Test Framework API](./README.md)**
    - [Synopsis](#synopsis)
    - [Tests are Functions](#tests-are-functions)
    - [Tests are Structures](#tests-are-structures)
    - [Tests can be Integrated](#tests-can-be-integrated)
    - [ui_test](./ui_test.md)
    - [ui_test_backtrace](./ui_test_backtrace.md)
    - [ui_test_expect](./ui_test_expect.md)
    - [ui_test_main](./ui_test_main.md)
    - [ui_test_options](./ui_test_options.md)
    - [ui_test_report](./ui_test_report.md)
    - [ui_test_status](./ui_test_status.md)

### Synopsis

Most test frameworks for C use a lot of macros to write functions for you. This can make it very difficult to debug.
This framework only uses macros that can cause errors at compile time. The main goal here is to have a framework
that doesn't obfuscate the code too much, and to avoid creating a domain-specific-language. You should still feel
like you're programming in C when you're writing your tests.

It should also be very easy to add tests, and you should be able to keep them side-by-side with the code that's
being tested. For example, if you have a file named `ui_feature.c`, you should be able to write your tests directly
inside of that file if you so choose, or just put a file named `ui_feature_test.c` in the same folder.

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
lot of times the horrible macro-generated test functions are what you end up seeing in the debugger when you run
into problems.

This framework solves that problem by allowing you to write your own test functions. Here's an example:

```c++
static void ui_test_runner ui_test_example (void);
```

The only macro used here, `ui_test_runner`, expands to an `__attribute__` or `_pragma`, depending on the compiler
you're using, that registers the function to run automatically.

There's not much hidden behind the preprocessor. Here's what the above expands to for GNU C:

```c++
static void __attribute__((constructor)) ui_test_example (void);
```

For MSVC, it's a little more complicated, but still not terrible:

```c++
static void __declspec (allocate (".CRT$XCU")) ui_test_example (void);
```

Still pretty straightforward what the intention is here: The function automatically runs at the start of the
test application, just before `main` is called.

### Tests are Structures

Of course, tt's not ideal to have the tests run before `main`, so, they run twice: Once at the test application's
startup to register the test, and again later by `main` to actually run the test. This is accomplished by creating a
static variable inside the function with information about the test, like this:

```c++
static struct ui_test test = ui_test (test, ui_test_example);
```

There is only one macro being used here, `ui_test`. There's a bit to unpack here, so let's just look at how all the
macros expand. This is exactly how you'd write the test without any macros (for GNU C, anyway):

```c++
static void __attribute__((constructor))
ui_test_example (void)
{
  static struct ui_test test = { UI_TEST_STATUS_REGISTER, { 0, __FILE__, __LINE__ }, 0 };

  if (ui_test_register (&test))
    return;
}
```

Here are the key-points that makes this work:

- When the application starts, `ui_test_example` is called sometime before `main`.
- The function creates the static `test` variable, with its `status` set to `UI_TEST_STATUS_REGISTER`.
- `ui_test_register` is then called, which checks `status`, sees that it's set to `UI_TEST_STATUS_REGISTER`,
  registers the test, and returns `1`, thereby ending the first run of `ui_test_example`.

Then, sometime during the lifetime of the test application, `ui_test_example` is called again to run the test. This
time is does this:

- The static `test` variable has already been instantiated, so the first line is essentially skipped.
- `ui_test_register` is called, which checks the test's `status`, sees that it isn't set to `UI_TEST_STATUS_REGISTER`,
  and so it pretty much immediately exits with `0`, allowing whatever other code you added to the test function to run.

And since you have access to the static struct `test`, as does the caller of the test function, you have two-way
communication with the test framework during the execution of your test functions.

Here's a final example of a test without any expanded macros:

```c++
#include <ui_test.h>
#include <ui_test_expect.h>

static void ui_test_runner
ui_test_example (void)
{
  static struct ui_test test = ui_test (test, ui_test_example);

  ui_expect (1 == 1, "1 != 1??");
}
```

It might be a bit more verbose than the typical macro-heavy unit-testing framework you're used to. But, hey,
nothing's hidden behind a preprocessor here. Well, not much, anyway.

### Tests can be Integrated

Because this system is so simple, you can actually integrate your tests directly into your library if you so choose.
For example, let's suppose you write an API with this in it for some reason:

```c++
int sum (int a, int b);
```

And your implementation looks like this:

```c++
int
sum (int a, int b)
{
  return a + b;
}
```

You can add your test to the same file, like this:

```c++
#include <ui_test.h>
#include <ui_test_expect.h>

int
sum (int a, int b)
{
  return a + b;
}

static void ui_test_runner
sum_test (void)
{
  static struct ui_test test = ui_test (test, sum_test);

  ui_expect (sum (1, 2) == 3, "1+2 should equal 3");
}
```

By default, the tests will be reported to the console, but you can easily implement your own test reporter if
you'd rather see them inside the application itself. For example, if you're writing acceptance tests that require
directly interacting with the GUI.

You don't have to worry about only including the tests for debug builds, either, because `ui_test_runner` defaults
to an empty statement for release builds, so the test functions, and the test framework itself by extension, will
all be optimized out of your release builds.
