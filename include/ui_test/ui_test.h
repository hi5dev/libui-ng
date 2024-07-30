#pragma once

#include "ui_test_reporter.h"

#ifndef UI_TEST_REGISTRY_CAPACITY
/**
 * @brief Maximum number of tests that can be registered.
 * @details This can be overridden before including <ui_test.h> to set a lower or higher limit.
 */
#define UI_TEST_REGISTRY_CAPACITY 1024
#endif

#if defined(__GNUC__)
#define ui_test(n)                                                                                                    \
  static void n (__attribute__ ((maybe_unused)) struct ui_test_t *test);                                              \
  static void __attribute__ ((constructor)) n##_register (void)                                                       \
  {                                                                                                                   \
    ui_test_register ({ .name = #n, .f = n, .filename = __FILE_NAME__, .line = __LINE__ });                           \
  }                                                                                                                   \
  void n (__attribute__ ((maybe_unused)) struct ui_test_t *test)

#elif defined(_MSC_VER)
#define ui_test(n)                                                                                                    \
  static void n (__attribute__ ((maybe_unused)) struct ui_test_t *test);                                              \
  static void n##_register (void)                                                                                     \
  {                                                                                                                   \
    ui_test_register ({ .name = #n, .f = n, .filename = __FILE_NAME__, .line = __LINE__ });                           \
  }                                                                                                                   \
  __pragma (section (".CRT$XCU", read)) __declspec (allocate (".CRT$XCU")) void (*n##__register) () = n##_register;   \
  void n (__attribute__ ((maybe_unused)) struct ui_test_t *test)

#else
#error "unsupported compiler"

#endif

/**
 * @brief Test runner function type.
 */
typedef void (ui_test_run_f) (struct ui_test_t *test);

/**
 * @brief Stores information about a test.
 */
struct ui_test_t
{
  /**
   * @brief The unit test's name.
   */
  const char *name;

  /**
   * @brief Test runner.
   */
  ui_test_run_f *f;

  /**
   * @brief Reason why the test failed, or @p NULL if the test passed.
   */
  const char *failure;

  /**
   * @brief Full path to the file the test is in.
   */
  const char *filename;

  /**
   * @brief The line number of the test declaration, or of the cause of @p failure when it is not @p NULL
   */
  int line;
};

/**
 * @brief @p ui_test_register parameters.
 */
struct ui_test_register_params
{
  /**
   * @brief Name of the test.
   * @remark This should be a C function friendly name.
   */
  const char *name;

  /**
   * @brief Test runner function.
   */
  ui_test_run_f *f;

  /**
   * @brief Full path to the file with the test function.
   */
  const char *filename;

  /**
   * @brief Line number inside @p filename at which the test is defined.
   */
  const int line;
};

/**
 * @brief Gets the global test reporter.
 * @return @p ui_test_get_reporter currently in use.
 */
struct ui_test_reporter_t *ui_test_get_reporter (void);

/**
 * @brief Registers a unit-test.
 * @param params @p ui_test_register_params
 */
void ui_test_register (struct ui_test_register_params params);

/**
 * @brief Runs all the unit tests in the order they were registered.
 */
void ui_test_run_all (void);

/**
 * @brief Runs one unit test.
 * @param test to run.
 */
void ui_test_run_one (struct test_t *test);

/**
 * @brief Runs all tests that matches a given name.
 * @param name of the test.
 */
void ui_test_run_by_name (const char *name);

/**
 * @brief Sets the global test reporter.
 * @param reporter to use.
 */
void ui_test_set_reporter (struct ui_test_reporter_t *reporter);
