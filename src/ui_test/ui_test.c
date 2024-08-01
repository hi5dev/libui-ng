#include <ui_test.h>
#include <ui_test_report.h>
#include <ui_test_report_event.h>

#include <assert.h>
#include <stddef.h>
#include <string.h>

#ifdef ui_test_register
#undef ui_test_register
#endif

/// test was previously registered and is ready to run
#define UI_TEST_READY 0

/// test was just registered and should not run
#define UI_TEST_REGISTERED 1

/// test was previously registered, but is not yet ready to run
#define UI_TEST_NOT_READY 2

/**
 * @brief Static storage for registering tests.
 */
static struct ui_test_t *ui_test_first = NULL;

int
ui_test_n_registered (void)
{
  if (ui_test_first == NULL)
    return 0;

  assert (ui_test_first->previous == NULL);
  if (ui_test_first->next == NULL)
    return 1;

  int count = 0;
  for (const struct ui_test_t *test = ui_test_first; test != NULL; test = test->next)
    ++count;

  assert (count > 0);
  return count;
}

int
ui_test_register (struct ui_test_t *test)
{
  assert (test != NULL);

  if (test->status > UI_TEST_STATUS_REGISTER)
    return test->status <= UI_TEST_STATUS_RUNNING ? UI_TEST_READY : UI_TEST_NOT_READY;

  assert (test->name != NULL);
  assert (test->run != NULL);

  assert (test->previous == NULL);
  assert (test->next == NULL);

  struct ui_test_t *first = ui_test_first;
  if (first != NULL)
    {
      assert (first->previous == NULL);
      first->previous = test;
      test->next      = first;
    }

  ui_test_first = test;

  test->status = UI_TEST_STATUS_PENDING;

  return UI_TEST_REGISTERED;
}

void
ui_test_report_dispatch (volatile struct ui_test_t *test, const enum ui_test_report_event_t event)
{
  assert (test != NULL);

  if (test->report != NULL && test->report->report_cb != NULL)
    test->report->report_cb ((struct ui_test_report_message_t){ event, test });
}

void
ui_test_run_all (struct ui_test_report_t *report, struct ui_test_options_t *options)
{
  for (struct ui_test_t *test = ui_test_first; test != NULL; test = test->next)
    ui_test_run_one (test, report, options);
}

void
ui_test_run_by_name (const char *name, struct ui_test_report_t *report, struct ui_test_options_t *options)
{
  assert (name != NULL);

  for (struct ui_test_t *test = ui_test_first; test != NULL; test = test->next)
    if (strcmp (name, test->name) == 0)
      ui_test_run_one (test, report, options);
}

void
ui_test_run_one (volatile struct ui_test_t *test, struct ui_test_report_t *report, struct ui_test_options_t *options)
{
  assert (test != NULL);
  assert (test->run != NULL);

  assert (test->status != UI_TEST_STATUS_REGISTER);
  assert (test->status != UI_TEST_STATUS_RUNNING);

  test->report  = report;
  test->options = options;

  ui_test_report_dispatch (test, UI_TEST_REPORT_EVENT_RUN);
  test->status = UI_TEST_STATUS_RUNNING;
  test->run ();

  if (test->status == UI_TEST_STATUS_RUNNING)
    test->status = UI_TEST_STATUS_PASSED;

  if (test->report != NULL)
    {
      test->report->n_tests_run++;

      if (test->status == UI_TEST_STATUS_PASSED)
        test->report->n_tests_passed++;

      else if (test->status == UI_TEST_STATUS_FAILED)
        test->report->n_tests_failed++;

      else if (test->status == UI_TEST_STATUS_SKIPPED)
        test->report->n_tests_skipped++;
    }

  ui_test_report_dispatch (test, UI_TEST_REPORT_EVENT_STOP);

  test->report  = NULL;
  test->options = NULL;
}

void
ui_test_set_status (struct ui_test_t *test, const enum ui_test_status_t status, const char *message,
                    const char *filename, const int line)
{
  assert (test != NULL);

  test->status             = status;
  test->backtrace.message  = message;
  test->backtrace.filename = filename;
  test->backtrace.line     = line;
}
