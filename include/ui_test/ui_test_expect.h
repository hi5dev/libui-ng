#pragma once

#include "ui_test.h"

/**
 * @brief Returns from the caller's scope if the given condition evaluates to false.
 * @param condition to evaluate.
 * @param ... optional return value.
 */
#define _ui_expect(condition, ...)                                                                                    \
  do                                                                                                                  \
    {                                                                                                                 \
      if (!(condition))                                                                                               \
        return __VA_ARGS__;                                                                                           \
    }                                                                                                                 \
  while (0)

/**
 * @brief Expects the given condition to evaluate to true.
 * @param condition to evaluate.
 * @param message to use when the expectation fails.
 * @remark Caller must have a @p ui_test_t accessible to the same scope as the caller named @p test
 * @remark The test's status is set to PASSED when the condition evaluates to true.
 */
#define ui_expect(condition, message)                                                                                 \
  do                                                                                                                  \
    {                                                                                                                 \
      if ((condition))                                                                                                \
        ui_test_set_status (&test, UI_TEST_STATUS_PASSED, 0, __FILE__, __LINE__);                                     \
      else                                                                                                            \
        ui_test_interrupt (UI_TEST_STATUS_FAILED, message);                                                           \
    }                                                                                                                 \
  while (0)

/**
 * @brief Interrupts the current test; updates its status and backtrace message.
 * @param status to update the test to
 * @param message to include in the backtrace or @p NULL to not include a backtrace in the report
 * @remark Caller must have a @p ui_test_t accessible to the same scope as the caller named @p test
 */
#define ui_test_interrupt(status, message)                                                                            \
  do                                                                                                                  \
    {                                                                                                                 \
      ui_test_set_status (&test, status, message, __FILE__, __LINE__);                                                \
      return;                                                                                                         \
    }                                                                                                                 \
  while (0)

/**
 * @brief Interrupts the current test and updates its status to PASSED; clears the backtrace message.
 * @remark Caller must have a @p ui_test_t accessible to the same scope as the caller named @p test
 */
#define ui_test_pass() ui_test_interrupt (UI_TEST_STATUS_PASSED, 0)

/**
 * @brief Interrupts the current test, marking it as "skipped" with a reason why it was skipped.
 * @param message reason why the test is being skipped.
 * @remark Skipped tests can be forced to continue by setting the verbose option.
 */
#define ui_test_skip(message)                                                                                         \
  do                                                                                                                  \
    {                                                                                                                 \
      if (!test.options->force_skipped_tests)                                                                         \
        ui_test_interrupt (UI_TEST_STATUS_SKIPPED, message);                                                          \
    }                                                                                                                 \
  while (0)

/**
 * @brief Expects two values of the same given type to either be equal or not equal.
 * @param type of the two values being compared.
 * @param l the actual value
 * @param is_or_is_not if the values being compared should be equal or not equal.
 * @param r the expected value
 */
#define ui_expect_cmp(type, l, is_or_is_not, r)                                                                       \
  _ui_expect (ui_test_cmp_##type (&test, (int)ui_test_##is_or_is_not, l, r, __FILE__, __LINE__))

/**
 * @brief Expects the given pointer to <b>be</b> @p NULL
 * @param ptr to check for @p NULL
 */
#define ui_expect_null(ptr) _ui_expect (ui_test_expect_null (&test, 0, #ptr, (void *)ptr, __FILE__, __LINE__))

/**
 * @brief Expects the given pointer <b>not</b> to be @p NULL
 * @param ptr to check for @p NULL
 */
#define ui_expect_not_null(ptr) _ui_expect (ui_test_expect_null (&test, 1, #ptr, ptr, __FILE__, __LINE__))

/**
 * @brief Used to determine if something is or is not.
 */
enum ui_test_is_or_is_not
{
  ui_test_is     = 0, //<! expected to be
  ui_test_is_not = 1, //<! expected not to be
};

/**
 * @brief Checks the equality of two doubles.
 * @param test that is running.
 * @param invert non-zero when the values should not be equal.
 * @param l the expected value
 * @param r the actual value
 * @param file full path to the caller's source file.
 * @param line line number of the caller.
 * @return @p 1 when @p invert is @p 0 and the difference between @p l and @p r <b>are</b> within @p DBL_EPSILON
 * @return @p 0 when @p invert is @p 1 and the difference between @p l and @p r <b>are</b> within @p DBL_EPSILON
 * @return @p 0 when @p invert is @p 0 and the difference between @p l and @p r are <b>not</b> within @p DBL_EPSILON
 * @return @p 1 when @p invert is @p 1 and the difference between @p l and @p r are <b>not</b> within @p DBL_EPSILON
 */
int ui_test_cmp_double (struct ui_test_t *test, int invert, double l, double r, const char *file, int line);

/**
 * @brief Checks the equality of two floats.
 * @param test that is running.
 * @param invert non-zero when the values should not be equal.
 * @param l the expected value
 * @param r the actual value
 * @param file full path to the caller's source file.
 * @param line line number of the caller.
 * @return @p 1 when @p invert is @p 0 and the difference between @p l and @p r <b>are</b> within @p FLT_EPSILON
 * @return @p 0 when @p invert is @p 1 and the difference between @p l and @p r <b>are</b> within @p FLT_EPSILON
 * @return @p 0 when @p invert is @p 0 and the difference between @p l and @p r are <b>not</b> within @p FLT_EPSILON
 * @return @p 1 when @p invert is @p 1 and the difference between @p l and @p r are <b>not</b> within @p FLT_EPSILON
 */
int ui_test_cmp_float (struct ui_test_t *test, int invert, float l, float r, const char *file, int line);

/**
 * @brief Checks the equality of two integers.
 * @param test that is running.
 * @param invert non-zero when the values should not be equal.
 * @param l the expected value
 * @param r the actual value
 * @param file full path to the caller's source file.
 * @param line line number of the caller.
 * @return non-zero when the value are equal and @p invert is zero, zero when the values are equal and @p invert is not
 * @return @p 1 when @p invert is @p 0 and the difference between @p l and @p r <b>is</b> @p 0
 * @return @p 0 when @p invert is @p 1 and the difference between @p l and @p r <b>is</b> @p 0
 * @return @p 0 when @p invert is @p 0 and the difference between @p l and @p r is <b>not</b> @p 0
 * @return @p 1 when @p invert is @p 1 and the difference between @p l and @p r is <b>not</b> @p 0
 */
int ui_test_cmp_int (struct ui_test_t *test, int invert, int l, int r, const char *file, int line);

/**
 * @brief Checks the equality of two strings.
 * @param test that is running.
 * @param invert non-zero when the values should not be equal.
 * @param l the expected value
 * @param r the actual value
 * @param file full path to the caller's source file.
 * @param line line number of the caller.
 * @return @p 1 when @p invert is @p 0 and @p l and @p r <b>do</b> match
 * @return @p 0 when @p invert is @p 1 and @p l and @p r <b>do</b> match
 * @return @p 0 when @p invert is @p 0 and @p l and @p r do <b>not</b> match
 * @return @p 1 when @p invert is @p 1 and @p l and @p r do <b>not</b> match
 */
int ui_test_cmp_str (struct ui_test_t *test, int invert, const char *l, const char *r, const char *file, int line);

/**
 * @brief Tests for @p NULL or not @p NULL
 * @param test that is running.
 * @param invert non-zero to expect @p NULL
 * @param ptr_name name of the pointer that should or should not be @p NULL.
 * @param actual pointer to check for @p NULL
 * @param file full path to the caller's source file.
 * @param line line number of the caller.
 * @return @p 1 when @p invert is @p 0 and @p actual <b>is</b> @p NULL
 * @return @p 0 when @p invert is @p 0 and @p actual is <b>not</b> @p NULL
 * @return @p 1 when @p invert is @p 1 and @p actual <b>is</b> @p NULL
 * @return @p 0 when @p invert is @p 1 and @p actual is <b>not</b> @p NULL
 */
int ui_test_expect_null (struct ui_test_t *test, int invert, const char *ptr_name, void *actual, const char *file,
                         int line);

/**
 * @brief Tests for true or false conditions.
 * @param test that is running.
 * @param invert non-zero to expect false.
 * @param actual non-zero when true.
 * @param file full path to the caller's source file.
 * @param line line number of the caller.
 * @return @p 1 when @p invert is @p 0 and @p actual is <b>non-zero</b>
 * @return @p 0 when @p invert is @p 0 and @p actual is <b>zero</b>
 * @return @p 1 when @p invert is @p 1 and @p actual is <b>non-zero</b>
 * @return @p 0 when @p invert is @p 1 and @p actual is <b>zero</b>
 */
int ui_test_expect_bool (struct ui_test_t *test, int invert, int actual, const char *file, int line);
