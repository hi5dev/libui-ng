#include <ui_test.h>

#include "ui_test_console_reporter.h"

#include <assert.h>
#include <stdio.h>

#define percent(total, value) ((double)value / (double)total * 100.0)

void
ui_test_console_reporter (const struct ui_test_report_message_t message)
{
  switch (message.event)
    {
    case UI_TEST_REPORT_EVENT_REGISTER:
      break;

    case UI_TEST_REPORT_EVENT_RUN:
      {
        (void)fprintf (stdout, "Running %s ... ", message.test->name);
        (void)fflush (stdout);
        break;
      }

    case UI_TEST_REPORT_EVENT_STOP:
      {
        switch (message.test->status)
          {
          case UI_TEST_STATUS_FAILED:
            {
              const struct ui_test_backtrace_t bt = message.test->backtrace;
              (void)fprintf (stdout, "[FAIL]\n  %s:%d: %s\n\n", bt.filename, bt.line, bt.message);
              (void)fflush (stdout);
              break;
            }

          case UI_TEST_STATUS_SKIPPED:
            {
              const struct ui_test_backtrace_t bt     = message.test->backtrace;
              const char                      *reason = bt.message == NULL ? "no reason given" : bt.message;
              (void)fprintf (stdout, "[SKIP] (%s)\n  %s:%d\n\n", reason, bt.filename, bt.line);
              (void)fflush (stdout);
              break;
            }

          case UI_TEST_STATUS_PASSED:
            {
              (void)fprintf (stdout, "[PASS]\n");
              (void)fflush (stdout);
              break;
            }

          default:
            _assert ("unexpected test status after stopping", __FILE__, __LINE__);
          }
      }

    default:;
    }
}

void
ui_test_console_reporter_print_summary (void)
{
  struct ui_test_report_t *r = NULL;
  ui_test_report (&r);
  if (r == NULL)
    return;

  const int n_total = r->n_tests_registered;

  // example: Ran 10 tests: 0 skipped (0.00%), 9 passed (90.00%), 1 failed (10.00%)
  (void)fprintf (stdout,                                                                  //
                 "\n%s %d %s %d %s: %d %s (%0.2f%%), %d %s (%0.2f%%), %d %s (%0.2f%%)\n", //
                 "Ran", r->n_tests_run, "of", r->n_tests_registered, "test(s)",           //
                 r->n_tests_skipped, "skipped", percent (n_total, r->n_tests_skipped),    //
                 r->n_tests_passed, "passed", percent (n_total, r->n_tests_passed),       //
                 r->n_tests_failed, "failed", percent (n_total, r->n_tests_failed)        //
  );
  (void)fflush (stdout);
}
