# Test Framework API: ui_test_expect

Test expectation macros and methods.

## [Index](../../README.md)
- [APIs](../README.md)
  - [Test Framework API](./README.md)
    - **[ui_test_expect](./ui_test_expect.md)**
      - [ui_expect](#ui_expect)
      - [_ui_expect](#_ui_expect)

### `ui_expect`

This macro is the equivilent of `assert`, except that it doesn't abort the process, and instead fails the current test.

```c++
ui_expect (x == 1, "x should equal 1");
```

> This macro expects a variable named `test` to exist in the same scope as the caller that points to an
> instance of `ui_test_t`.

### `_ui_expect`

This function is used the same way `_assert` is used by `assert`. It's used by expectation macros to fail a test
when the expectation evaluates to false.

```c++
/**
 * @brief Expects @p l and @p r to be equal using @p == for comparison.
 * @param l left-side of the comparison
 * @param r right-side of the comparison
 * @remark Expects a variable named `test` that points to a @p ui_test_t
 * instance to exist in the same scope as the caller.
 */
#define ui_expect_equals(l, r)                                                \
  do                                                                          \
    {                                                                         \
      if (l == r)                                                             \
        break;                                                                \
      else                                                                    \
        _ui_expect (test, "Expected " #l " == " #r, __FILE_NAME__, __LINE__); \
    }                                                                         \
  while (0)
```
