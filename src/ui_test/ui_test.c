#include <ui_test.h>
#include <ui_test_report.h>

#include <stddef.h>
#include <string.h>

#ifdef ui_test_register
#undef ui_test_register
#endif

struct ui_test_t *ui_test_registry = NULL;

static void
ui_test_report_ensure_default (void)
{
  static struct ui_test_report_t default_report = { 0 };

  struct ui_test_report_t *report = NULL;

  ui_test_report (&report);

  if (report != NULL)
    return;

  report = &default_report;

  ui_test_report (&report);
}

static struct ui_test_t *
ui_test_first (void)
{
  struct ui_test_t *test = ui_test_registry;

  while (test != NULL && test->previous != NULL)
    test = test->previous;

  return test;
}

void
ui_test_run_all (struct ui_test_options_t *options)
{
  for (struct ui_test_t *test = ui_test_first (); test != NULL; test = test->next)
    ui_test_run_one (test, options);
}

void
ui_test_run_by_name (const char *name, struct ui_test_options_t *options)
{
  for (struct ui_test_t *test = ui_test_first (); test != NULL; test = test->next)
    if (strcmp (name, test->name) == 0)
      ui_test_run_one (test, options);
}

void
ui_test_run_one (struct ui_test_t *test, struct ui_test_options_t *options)
{
  if (test == NULL || test->run == NULL)
    return;

  test->backtrace.message = NULL;
  test->status            = UI_TEST_STATUS_RUNNING;

  ui_test_report_dispatch (UI_TEST_REPORT_EVENT_RUN, test);

  test->options = options;
  test->run ();

  if (test->status == UI_TEST_STATUS_RUNNING)
    test->status = UI_TEST_STATUS_PASSED;

  ui_test_report_dispatch (UI_TEST_REPORT_EVENT_STOP, test);
}

int
ui_test_register (struct ui_test_t *test, struct ui_test_options_t *options)
{
  if (test->status != UI_TEST_STATUS_REGISTER)
    return 0;

  ui_test_report_ensure_default ();

  struct ui_test_t *last = ui_test_registry;

  while (last != NULL && last->next != NULL)
    last = last->next;

  if (last == NULL)
    ui_test_registry = test;

  else
    last->next = test;

  test->status = UI_TEST_STATUS_PENDING;

  ui_test_report_dispatch (UI_TEST_REPORT_EVENT_REGISTER, test);

  return 1;
}

void
ui_test_set_status (struct ui_test_t *test, const enum ui_test_status_t status, const char *message,
                    const char *filename, const int line)
{
  test->status             = status;
  test->backtrace.message  = message;
  test->backtrace.filename = filename;
  test->backtrace.line     = line;
}
