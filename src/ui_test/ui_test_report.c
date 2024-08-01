#include <ui_test.h>
#include <ui_test_report.h>

#include <assert.h>
#include <stddef.h>

void
ui_test_report_cb (ui_test_report_cb_t **callback)
{
  static ui_test_report_cb_t *registered_callback;

  if (callback == NULL)
    registered_callback = NULL;

  else if (*callback == NULL)
    *callback = registered_callback;

  else
    registered_callback = *callback;
}

void
ui_test_report (struct ui_test_report_t **report)
{
  static struct ui_test_report_t *registered_report;

  if (report == NULL)
    registered_report = NULL;

  else if (*report == NULL)
    *report = registered_report;

  else
    registered_report = *report;
}

void
ui_test_report_dispatch (const enum ui_test_report_event_t event, struct ui_test_t *test)
{
  struct ui_test_report_t *report = NULL;
  ui_test_report (&report);
  if (report != NULL)
    switch (event)
      {
      case UI_TEST_REPORT_EVENT_REGISTER:
        {
          report->n_tests_registered++;
          break;
        }

      case UI_TEST_REPORT_EVENT_STOP:
        {
          switch (test->status)
            {
            case UI_TEST_STATUS_PASSED:
              report->n_tests_passed++;
              break;

            case UI_TEST_STATUS_FAILED:
              report->n_tests_failed++;
              break;

            case UI_TEST_STATUS_SKIPPED:
              report->n_tests_skipped++;
              break;

            default:
              _assert ("should not be reached", __FILE__, __LINE__);
            }

          break;
        }

      case UI_TEST_REPORT_EVENT_RUN:
        {
          report->n_tests_run++;
          break;
        }

      default:;
        _assert ("should not be reached", __FILE__, __LINE__);
      }

  ui_test_report_cb_t *callback = NULL;
  ui_test_report_cb (&callback);
  if (callback != NULL)
    callback ((struct ui_test_report_message_t){ .event = event, .report = report, .test = test });
}
