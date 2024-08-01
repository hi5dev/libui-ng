#pragma once

#include "ui_test_report_message.h"

/**
 * @brief Callback function-type for report events.
 * @param message
 */
typedef void (ui_test_report_cb_t) (struct ui_test_report_message_t message);

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

  /**
   * @brief Callback to notify of changes to the report.
   */
  ui_test_report_cb_t *report_cb;
};
