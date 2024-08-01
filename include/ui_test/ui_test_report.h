#pragma once

/**
 * @brief Test report notification events.
 */
enum ui_test_report_event_t
{
 UI_TEST_REPORT_EVENT_REGISTER = 0, //!< A test was just registered.
 UI_TEST_REPORT_EVENT_RUN      = 1, //!< A test is about to run.
 UI_TEST_REPORT_EVENT_STOP     = 2, //!< A test just stopped running.
};

/**
 * @brief Test report data.
 */
struct ui_test_report_t
{
  /**
   * @brief Total number of tests currently registered.
   */
  int n_tests_registered;

  /**
   * @brief Total number of tests that have run so far.
   * @remark Includes tests that have both passed and failed.
   */
  int n_tests_run;

  /**
   * @brief Total number of tests that were interrupted due to a failed expectation.
   */
  int n_tests_failed;

  /**
   * @brief Total number of tests that passed.
   */
  int n_tests_passed;

  /**
   * @brief Total number of tests that were skipped.
   */
  int n_tests_skipped;
};

/**
 * @brief Information about a test event - e.g. a test is going to run, or a test passed.
 */
struct ui_test_report_message_t
{
  /**
   * @brief Event that triggered the notification.
   */
  enum ui_test_report_event_t event;

  /**
   * @brief The updated test report.
   */
  struct ui_test_report_t *report;

  /**
   * @brief The test that triggered the event.
   */
  struct ui_test_t *test;
};

/**
 * @brief Test report callback function type.
 * @param message information about the event.
 * @remark Only one callback can be registered at a time.
 */
typedef void (ui_test_report_cb_t) (struct ui_test_report_message_t message);

/**
 * @brief Gets or sets the current test report callback.
 * @param[in,out] callback Used to get, set, or clear the current callback.
 * - @p NULL clears/unregisters the current callback,
 * - @p *callback is set to the current callback when it is a pointer to @p NULL,
 * - Otherwise the current callback is reassigned to its dereferenced value.
 */
void ui_test_report_cb (ui_test_report_cb_t **callback);

/**
 * @brief Gets or sets the current test report.
 * @param[in,out] report Used to get, set, or clear the current test report.
 * - @p NULL clears/unregisters the current test report,
 * - @p *report is set to the current test report when it is a pointer to @p NULL,
 * - Otherwise the current test report is reassigned to its dereferenced value.
 */
void ui_test_report (struct ui_test_report_t **report);

/**
 * @brief Updates the current test report and dispatches a message to the registered callback.
 * @param event to dispatch.
 * @param test that triggered the event.
 */
void ui_test_report_dispatch (enum ui_test_report_event_t event, struct ui_test_t *test);
