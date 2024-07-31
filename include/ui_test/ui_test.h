#pragma once

#if defined(__GNUC__)
#define ui_test_constructor __attribute__ ((constructor))
#elif defined(_MSC_VER)
#define ui_test_constructor __pragma (section (".CRT$XCU", read)) __declspec (allocate (".CRT$XCU"))
#endif

/**
 * @brief Initializes a @p ui_test_t that is prepared for registration.
 * @param n the test function.
 */
#define ui_test(n) ui_test_init (#n, n, __FILE__, __LINE__)

/**
 * @brife Information about a test.
 */
struct ui_test_t;

/**
 * @brief Test statuses.
 */
enum ui_test_status_t
{
  UI_TEST_STATUS_REGISTER = 0, //<! The test is being registered.
  UI_TEST_STATUS_PENDING  = 1, //<! Still waiting to run.
  UI_TEST_STATUS_RUNNING  = 2, //<! Currently in progress.
  UI_TEST_STATUS_PASSED   = 3, //<! Completed successfully.
  UI_TEST_STATUS_FAILED   = 4, //<! Was interrupted due to a failed expectation.
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
 * @brief Initializes a @p ui_test_t on the stack.
 * @param name of the test.
 * @param run callback.
 * @param filename of the source file where the test is defined.
 * @param line number of the test's definition in @p filename.
 * @return @p ui_test_t
 */
struct ui_test_t ui_test_init (const char *name, ui_test_cb_t *run, const char *filename, int line);

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
 * @return non-zero if the test was registered, zero if it was already registered before calling this function.
 */
int ui_test_register (struct ui_test_t *test);

#define ui_test_register(t) if (ui_test_register (&test)) return;
