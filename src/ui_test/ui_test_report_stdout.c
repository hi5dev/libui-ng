#include <ui_test.h>
#include <ui_test_report_stdout.h>

#include <assert.h>
#include <stdio.h>

#define percent(total, value) ((double)value / (double)total * 100.0)

#define DO_NOT_INCLUDE_MESSAGE 0
#define INCLUDE_MESSAGE        1

#define IS_NOT_VERBOSE 0
#define IS_VERBOSE     1

static void
ui_test_report_stdout_backtrace (const struct ui_test_report_message_t *message, const int include_message,
                                 const int is_verbose)
{
  if (message->test == NULL)
    return;

  (void)fprintf (stdout, "\n");

  const volatile struct ui_test_options_t *options = message->test->options;

  const int verbose = options == NULL ? 0 : options->verbose;
  if (is_verbose && !verbose)
    return;

  const int silence_backtrace = options == NULL ? 0 : options->silence_backtraces;
  if (silence_backtrace && !verbose)
    return;

  const volatile struct ui_test_backtrace_t *bt = &message->test->backtrace;
  if (bt == NULL)
    return;

  if (include_message)
    (void)fprintf (stdout, "  %s:%d: %s\n\n", bt->filename, bt->line, bt->message);

  else
    (void)fprintf (stdout, "  %s:%d\n\n", bt->filename, bt->line);
}

static void
ui_test_report_stdout_failed (const struct ui_test_report_message_t *message)
{
  (void)fprintf (stdout, "[FAIL]");

  ui_test_report_stdout_backtrace (message, INCLUDE_MESSAGE, IS_NOT_VERBOSE);

  (void)fflush (stdout);
}

static void
ui_test_report_stdout_passed (const struct ui_test_report_message_t *message)
{
  (void)fprintf (stdout, "[PASS]");

  ui_test_report_stdout_backtrace (message, INCLUDE_MESSAGE, IS_VERBOSE);

  (void)fflush (stdout);
}

static void
ui_test_report_stdout_run (const struct ui_test_report_message_t *message)
{
  if (message->test == NULL)
    return;

  (void)fprintf (stdout, "Running %s ... ", message->test->name);

  (void)fflush (stdout);
}

static void
ui_test_report_stdout_skipped (const struct ui_test_report_message_t *message)
{
  const volatile struct ui_test_backtrace_t *bt = &message->test->backtrace;

  if (bt->message == NULL)
    (void)fprintf (stdout, "[SKIP]");

  else
    (void)fprintf (stdout, "[SKIP] (%s)", bt->message);

  ui_test_report_stdout_backtrace (message, DO_NOT_INCLUDE_MESSAGE, IS_NOT_VERBOSE);

  (void)fflush (stdout);
}

static void
ui_test_report_stdout_event_stop (const struct ui_test_report_message_t *message)
{
  switch (message->test->status)
    {
    case UI_TEST_STATUS_FAILED:
      ui_test_report_stdout_failed (message);
      break;

    case UI_TEST_STATUS_SKIPPED:
      ui_test_report_stdout_skipped (message);
      break;

    case UI_TEST_STATUS_PASSED:
      ui_test_report_stdout_passed (message);
      break;

    default:
      _assert ("unexpected test status reported", __FILE__, __LINE__);
    }
}

void
ui_test_report_stdout_cb (const struct ui_test_report_message_t message)
{
  switch (message.event)
    {
    case UI_TEST_REPORT_EVENT_RUN:
      ui_test_report_stdout_run (&message);
      break;

    case UI_TEST_REPORT_EVENT_STOP:
      ui_test_report_stdout_event_stop (&message);
      break;

    default:;
      _assert ("unhandled report event", __FILE__, __LINE__);
    }
}

void
ui_test_report_stdout_print_summary (const struct ui_test_report_t *report)
{
  if (report == NULL)
    return;

  if (report->n_tests_run == 0)
    {
      (void)fprintf (stdout, "No tests ran. Perhaps there were no defined expectations?\n");
      return;
    }

  const int n_run        = report->n_tests_run;
  const int n_registered = report->n_tests_registered;
  const int n_skipped    = report->n_tests_skipped;
  const int n_passed     = report->n_tests_passed;
  const int n_failed     = report->n_tests_failed;

  // example: Ran 10 tests: 0 skipped (0.00%), 9 passed (90.00%), 1 failed (10.00%)
  (void)fprintf (stdout,                                                                  //
                 "\n%s %d %s %d %s: %d %s (%0.2f%%), %d %s (%0.2f%%), %d %s (%0.2f%%)\n", //
                 "Ran", n_run, "of", n_registered, "test(s)",                             //
                 n_skipped, "skipped", percent (n_run, n_skipped),                        //
                 n_passed, "passed", percent (n_run, n_passed),                           //
                 n_failed, "failed", percent (n_run, n_failed)                            //
  );

  (void)fflush (stdout);
}
