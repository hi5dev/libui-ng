#pragma once

#include "ui_test_report_event.h"

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
   * @brief The test that triggered the event.
   */
  volatile struct ui_test_t *test;
};
