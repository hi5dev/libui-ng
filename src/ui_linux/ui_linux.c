#include <ui_linux.h>

#include <ui_test.h>
#include <ui_test_expect.h>

void
ui_linux_dispatch (struct ui_linux_t *)
{
}

void
ui_linux_main_update (struct ui_linux_t *)
{
}

static ui_test_case
ui_linux_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_linux_test);

  ui_test_skip ("TODO");
}
