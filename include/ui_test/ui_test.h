#pragma once

#include "ui_test_backtrace.h"
#include "ui_test_options.h"
#include "ui_test_report.h"
#include "ui_test_status.h"

/**
 * @def ui_test_case
 * @brief Registers a test runner function.
 * @example
 * @code
 * static ui_test_case
 * ui_example_test (void)
 * {
 *   static struct ui_test_t test = ui_test (test, ui_example_test);
 * }
 * @endcode
 * @remark Functions that use this macro will be marked as unused for release builds so they can be optimized out.
 */
#ifdef NDEBUG
#define ui_test_case void __attribute__ ((unused))
#else
#if defined(__GNUC__)
#define ui_test_case void __attribute__ ((constructor))
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU", read)
#define ui_test_case void __declspec (allocate (".CRT$XCU"))
#endif
#endif

/**
 * @def ui_test
 * @brief Initializes a @p ui_test_t and registers it.
 * @param var name of the test variable being defined.
 * @param runner callback function - also used as the test's name
 * @return @p ui_test_t
 * @example
 * @code
 * static struct ui_test_t test = ui_test (test, ui_example_test);
 * @endcode
 * @remark Tests do not run for release builds.
 */
#ifdef NDEBUG
#define ui_test(var, runner)                                                                                          \
  { UI_TEST_STATUS_SKIPPED, ui_test_backtrace (0), #runner, runner, 0 };                                              \
  return
#else
#define ui_test(var, runner)                                                                                          \
  { UI_TEST_STATUS_REGISTER, ui_test_backtrace (0), #runner, runner, 0 };                                             \
  if (ui_test_register (&var))                                                                                        \
  return
#endif

/**
 * @brief Test callback function type.
 */
typedef void (ui_test_cb_t) (void);

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
   * @brief Test options.
   */
  volatile struct ui_test_options_t *options;

  /**
   * @brief Current @p ui_test_report_t to notify of changes to the test's state.
   */
  volatile struct ui_test_report_t *report;

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
 * @return Total number of tests registered.
 */
int ui_test_n_registered (void);

/**
 * @brief Registers a test.
 * @param test to register.
 * @return non-zero if the test was registered, zero if it was already registered before calling this function.
 * @retval 0 when the caller can safely perform the test.
 * @retval 1 when the test was registered by the current call.
 * @retval 2 when the test was registered by a previous call, but is not in an appropriate state to run.
 */
int ui_test_register (struct ui_test_t *test);

/**
 * @brief Dispatches an event to its assigned report.
 * @param test with the report.
 * @param event to report.
 */
void ui_test_report_dispatch (volatile struct ui_test_t *test, enum ui_test_report_event_t event);

/**
 * @brief Runs all the registered tests.
 * @param report to use while running each test.
 * @param options @p ui_test_options_t
 */
void ui_test_run_all (struct ui_test_report_t *report, struct ui_test_options_t *options);

/**
 * @brief Runs all tests that match the given name.
 * @param name of the tests to run.
 * @param report to use while running the test.
 * @param options @p ui_test_options_t
 */
void ui_test_run_by_name (const char *name, struct ui_test_report_t *report, struct ui_test_options_t *options);

/**
 * @brief Runs one specific test.
 * @param test to run.
 * @param report to use while running the test.
 * @param options @p ui_test_options_t
 */
void ui_test_run_one (volatile struct ui_test_t *test, struct ui_test_report_t *report,
                      struct ui_test_options_t *options);

/**
 * @brief Updates the status and backtrace of a @p ui_test_t
 * @param test @p ui_test_t
 * @param status @p ui_test_status_t
 * @param message to include in the backtrace or @p NULL to ignore the backtrace when reporting the test result.
 * @param filename full path to the file with the test's source code.
 * @param line number that resulted in changing the test's status.
 */
void ui_test_set_status (struct ui_test_t *test, enum ui_test_status_t status, char *message,
                         const char *filename, int line);
