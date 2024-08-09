#pragma once

#include <ui_test.h>
#include <ui_test_compare.h>

#pragma region macros

#define ui_test_assert_compare(test, actual, expected, invert, file, func, line)                                      \
  ({                                                                                                                  \
    const void *_actual   = (const void *)actual;                                                                    \
    const void *_expected = (const void *)expected;                                                                  \
    ui_test_assert_init (test, file, func, line,                                                                      \
                         ui_test_compare_init (_actual, _expected, invert, ui_test_compare_function (actual),         \
                                               ui_test_compare_message (actual)));                                    \
  })

#define ui_assert_equality(actual, expected, invert, file, func, line)                                                \
  ui_test_return_if_not (ui_test_assert (ui_test_assert_compare (&test, actual, expected, invert, file, func, line)))

#define ui_assert_eq(actual, equal) ui_assert_equality (actual, equal, 0, __FILE__, __FUNCTION__, __LINE__)

#define ui_assert_not_eq(actual, not_equal) ui_assert_equality (actual, not_equal, 1, __FILE__, __FUNCTION__, __LINE__)

#pragma endregion

#pragma region types

/**
 * @brief Test assertion data.
 */
struct ui_test_assert_t;

#pragma endregion

#pragma region structure

/**
 * @brief Test assertion data.
 */
struct ui_test_assert_t
{
  /// @brief Test case.
  struct ui_test_t *test;

  /// @brief Source file.
  const char *file;

  /// @brief Function name.
  const char *func;

  /// @brief Line number.
  const int line;

  /// @brief Values being compared.
  struct ui_test_compare_t compare;
};

struct ui_test_assert_t ui_test_assert_init (struct ui_test_t *test, const char *file, const char *func, int line,
                                             struct ui_test_compare_t compare);

#pragma endregion

#pragma region functions

/**
 * @brief Performs the test described in the given assertion data and updates its test.
 * @param data @p ui_test_assert_t
 * @return non-zero when the test passes.
 */
int ui_test_assert (struct ui_test_assert_t data);

#pragma endregion
