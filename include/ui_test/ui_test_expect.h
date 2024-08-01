#pragma once

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
 * @brief Interrupts the test without an error, leaving the given explanation in the backtrace.
 * @param message reason why the test was skipped.
 * @remark Caller must have a @p ui_test_t accessible to the same scope as the caller named @p test
 */
#define ui_test_skip(message) ui_test_interrupt (UI_TEST_STATUS_SKIPPED, message)
