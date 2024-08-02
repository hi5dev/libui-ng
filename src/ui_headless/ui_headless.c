#include <ui_headless.h>

#include <ui_test.h>
#include <ui_test_expect.h>

void
ui_headless_dispatch (struct ui_headless_t *)
{
}

void
ui_headless_update (struct ui_headless_t *)
{
}

static ui_test_case
ui_headless_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_headless_test);

  ui_test_skip ("TODO");
}
