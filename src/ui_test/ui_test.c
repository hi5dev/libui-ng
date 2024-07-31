#include <ui_test.h>
#include <ui_test_report.h>

#include <stddef.h>
#include <string.h>

#ifdef ui_test_register
#undef ui_test_register
#endif

struct ui_test_t *ui_test_registry = NULL;

static struct ui_test_t *
ui_test_first (void)
{
  struct ui_test_t *test = ui_test_registry;

  while (test != NULL && test->previous != NULL)
    test = test->previous;

  return test;
}

struct ui_test_t
ui_test_init (const char *name, ui_test_cb_t *run, const char *filename, const int line)
{
  return {
    .status    = UI_TEST_STATUS_REGISTER,
    .backtrace = { .message = NULL, .filename = filename, .line = line },
    .name      = name,
    .run       = run,
    .previous  = NULL,
    .next      = NULL,
  };
}

void
ui_test_run_all (void)
{
  for (struct ui_test_t *test = ui_test_first (); test != NULL; test = test->next)
    ui_test_run_one (test);
}

void
ui_test_run_by_name (const char *name)
{
  for (struct ui_test_t *test = ui_test_first (); test != NULL; test = test->next)
    if (strcmp (name, test->name) == 0)
      ui_test_run_one (test);
}

void
ui_test_run_one (struct ui_test_t *test)
{
  if (test == NULL || test->run == NULL)
    return;

  test->backtrace.message = NULL;
  test->status            = UI_TEST_STATUS_RUNNING;

  ui_test_report_dispatch (UI_TEST_REPORT_EVENT_RUN, test);

  test->run (test);

  test->status = test->backtrace.message == NULL ? UI_TEST_STATUS_PASSED : UI_TEST_STATUS_FAILED;

  ui_test_report_dispatch (UI_TEST_REPORT_EVENT_STOP, test);
}

int
ui_test_register (struct ui_test_t *test)
{
  if (test->status != UI_TEST_STATUS_REGISTER)
    return 0;

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
