#pragma once

/**
 * @brief Test events that are reported.
 */
enum ui_test_report_event_t
{
  UI_TEST_REPORT_EVENT_RUN       = 0, //!< A test is about to run.
  UI_TEST_REPORT_EVENT_STOP      = 1, //!< A test just stopped running.
  UI_TEST_REPORT_EVENT_BACKTRACE = 2, //!< A test's backtrace was just updated.
};
