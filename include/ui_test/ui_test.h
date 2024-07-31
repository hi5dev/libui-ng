#pragma once

/**
 * @brief Test statuses.
 */
enum ui_test_status_t
{
  UI_TEST_STATUS_PENDING = 0, //<! Still waiting to run.
  UI_TEST_STATUS_RUNNING = 1, //<! Currently in progress.
  UI_TEST_STATUS_PASSED  = 2, //<! Completed successfully.
  UI_TEST_STATUS_FAILED  = 3, //<! Was interrupted due to a failed expectation.
};

/**
 * @brief Backtrace information for failed tests.
 */
struct ui_test_backtrace_t
{
  /**
   * @brief Reason the test failed, or @p NULL if the test hasn't failed.
   */
  const char *message;

  /**
   * @brief Full path to the source file with the test.
   */
  const char *filename;

  /**
   * @brief Line in the source where the test is defined, or of the last failure.
   */
  int line;
};

/**
 * @brief Test callback function type.
 * @param test that triggered the callback.
 */
typedef void (ui_test_cb_t) (struct ui_test_t *test);

/**
 * @brief Information about a test.
 */
struct ui_test_t
{
  /**
   * @brief The test's current status.
   */
  enum ui_test_status_t status;

  /**
   * @brief Backtrace information, mainly for failed tests.
   */
  struct ui_test_backtrace_t backtrace;

  /**
   * @brief A C function-friendly name of the test.
   */
  const char *name;

  /**
   * @brief Callback to invoke when running the test.
   */
  ui_test_cb_t *run;

  /**
   * @brief The previous test or @p NULL for the first registered test.
   */
  struct ui_test_t *previous;

  /**
   * @brief The next test or @p NULL for the last registered test.
   */
  struct ui_test_t *next;
};

/**
 * @brief Runs all the registered tests.
 */
void ui_test_run_all (void);

/**
 * @brief Runs all tests that match the given name.
 * @param name of the tests to run.
 */
void ui_test_run_by_name (const char *name);

/**
 * @brief Runs one specific test.
 * @param test to run.
 */
void ui_test_run_one (struct ui_test_t *test);

/**
 * @brief Registers a test.
 * @param test to register.
 */
void ui_test_register (struct ui_test_t *test);

#ifndef ui_test
#if defined(__GNUC__)
#define ui_test(n)                                                                                                    \
  static void n (__attribute__ ((maybe_unused)) struct ui_test_t *test);                                              \
  static void __attribute__ ((constructor)) n##_register (void)                                                       \
  {                                                                                                                   \
    ui_test_register ({ .name = #n, .run = n, .filename = __FILE_NAME__, .line = __LINE__ });                         \
  }                                                                                                                   \
  void n (__attribute__ ((maybe_unused)) struct ui_test_t *test)

#elif defined(_MSC_VER)
#define ui_test(n)                                                                                                    \
  static void n (__attribute__ ((maybe_unused)) struct ui_test_t *test);                                              \
  static void n##_register (void)                                                                                     \
  {                                                                                                                   \
    ui_test_register ({ .name = #n, .run = n, .filename = __FILE_NAME__, .line = __LINE__ });                         \
  }                                                                                                                   \
  __pragma (section (".CRT$XCU", read)) __declspec (allocate (".CRT$XCU")) void (*n##__register) () = n##_register;
\ void
n (__attribute__ ((maybe_unused)) struct ui_test_t *test)

#else
#error "unsupported compiler"

#endif
#endif
