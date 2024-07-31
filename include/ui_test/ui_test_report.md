# Test Framework API: ui_test_report

Test reporting API.

## [Index](../../README.md)
- [APIs](../README.md)
  - [Test Framework API](./README.md)
    - **[ui_test_report](./ui_test_report.md)**
      - [Synopsis](#synopsis)
      - [Example](#example)
      - [Types](#types)
      - [Functions](#functions)
      - [Implementation](../../src/ui_test/ui_test_report.c)

### Synopsis

Keeps a tally of the total number of registered tests, how many have run, and how many have failed. Includes a
callback for receiving test-event notifications.

### Example

```c++
#include <ui_test.h>
#include <ui_test_report.h>

#include <assert.h>
#include <malloc.h>
#include <stdef.h>
#include <stdio.h>

/// The callback is typically used to report to the terminal, log to a file, notify an IDE or an API for a CI.
static
void on_test_event (struct ui_test_report_message_t message)
{
  switch (message)
    {
      case UI_TEST_REPORT_EVENT_REGISTER:
        {
          // message.test was just registered
          break;
        }

      case UI_TEST_REPORT_EVENT_FAIL:
        {
          // message.test was interrupted due to a failed expectation
          (void)fprintf (stdout, "[%s]\n", "FAIL");
          (void)fprintf (stderr, "  %s\n", message.test->failure);
          break;
        }

      case UI_TEST_REPORT_EVENT_PASS:
        {
          // message.test successfully completed
          (void)fprintf (stdout, "[%s]\n", "PASS");
          break;
        }

      case UI_TEST_REPORT_EVENT_RUN:
        {
          // message.test is about to run
          (void)fprintf (stdout, "%s %s ... ", "Running", message.test->name);
          break;
        }

      default:
        _assert ("unhandled test report event", __FILE_NAME__, __LINE__);
    }
}

/// This isn't part of ui_test_report, but for the sake of completion, you'll likely want to include a summary.
static void
ui_test_report_print_summary (struct ui_test_report_t *report)
{
  (void)fprintf (stdout,

    // example: Ran 150 tests - 148 passed, 2 failed
    "%s %d %s - %d %s, %d %s",

    // ran n test(s)
    "Ran", report->n_tests_run, report->n_tests_run > 1 ? "tests" : "test",

    // n passed
    report->n_tests_run - report->n_tests_failed, "passed",

    // n failed
    report->n_tests_failed, "failed");
}

int
main (void)
{
  // Create the report.
  struct ui_test_report_t *report = calloc (1, sizeof (*report));

  // Register the report and callback function.
  ui_test_report (&report);
  ui_test_report_cb (&on_test_event);

  // Run all the registered tests.
  ui_test_run_all ();

  // Print the summary.
  ui_test_report_print_summary (report);

  // Exit with a failure if any tests failed.
  const int exit_code = report->n_tests_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;

  // Unregister the report and callback.
  ui_test_report_cb (NULL);
  ui_test_report (NULL);

  // Free the memory used by the report.
  free (report);

  // Return 0 when all tests passed.
  return exit_code;
}
```

### Types

| Type     | Name                       | Description                         |
|:---------|:---------------------------|:------------------------------------|
| struct   | `ui_test_report_t`         | Test report data.                   |
| function | `ui_test_report_cb_t`      | Test report callback function type. |
| struct   | `ui_test_report_message_t` | Test report callback data.          |
| enum     | `ui_test_report_event_t`   | Test report event types.            |

### Functions

| Returns | Name                      | Description                                              |
|:--------|:--------------------------|:---------------------------------------------------------|
| `void`  | `ui_test_report`          | Gets or sets the current test report.                    |
| `void`  | `ui_test_report_cb`       | Gets or sets the current test report callback.           |
| `void`  | `ui_test_report_dispatch` | Updates the test report and dispatches an event message. |
