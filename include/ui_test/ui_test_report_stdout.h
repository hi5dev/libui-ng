#pragma once

#include "ui_test_report_message.h"

/**
 * @brief Records changes to the report to @p stdout or @p stderr, depending on the severity of the event.
 * @param message to report.
 */
void ui_test_report_stdout_cb (struct ui_test_report_message_t message);

/**
 * @brief Prints a summary of the given report.
 * @details Example output:
 * @code{.text
 * Ran 10 tests: 0 skipped (0.00%), 9 passed (90.00%), 1 failed (10.00%)
 * @endcode
 * @param report to summarize.
 */
void ui_test_report_stdout_print_summary (const struct ui_test_report_t *report);
