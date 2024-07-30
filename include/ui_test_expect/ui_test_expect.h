#pragma once

#include <ui_test.h>

/**
 * @brief Expects the given condition to evaluate to true.
 * @param condition to evaluate.
 * @param message to use when the expectation fails.
 * @remark Must be called from with a scope with a @p void return type - e.g. @code void example_test (void); @endcode
 * @remark Caller must have a pointer to a @p ui_test_t named @p test already instantiated.
 */
#define ui_expect(condition, message)                                                                                 \
  do                                                                                                                  \
    {                                                                                                                 \
      if (!(condition))                                                                                               \
        {                                                                                                             \
          _ui_expect (test, message, __FILE_NAME__, __LINE__);                                                        \
          return;                                                                                                     \
        }                                                                                                             \
    }                                                                                                                 \
  while (0)

/**
 * @brief Updates a test's backtrace
 * @param test that failed
 * @param message with details regarding the failed expectation.
 * @param filename source file.
 * @param line with the failed expectation.
 */
void _ui_expect (struct ui_test_t *test, const char *message, const char *filename, int line);
