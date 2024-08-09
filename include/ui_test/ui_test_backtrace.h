#pragma once

/**
 * @brief Initializes a @p ui_test_backtrace_t using the filename and line of the caller.
 * @param message or @p NULL
 */
#define ui_test_backtrace(message)                                                                                    \
  {                                                                                                                   \
    message, __FILE__, __func__, __LINE__                                                                             \
  }

/**
 * @brief Backtrace information for tests.
 */
struct ui_test_backtrace_t
{
  /// @brief Reason the test failed, or @p NULL if the test hasn't failed.
  char *message;

  /// @brief Full path to the source file with the test.
  const char *file;

  /// @brief Test case function.
  const char *func;

  /// @brief Line in the source where the test is defined, or of the last failure.
  int line;
};
