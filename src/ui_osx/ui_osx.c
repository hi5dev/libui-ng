#include <ui_osx.h>
#include <ui_test.h>
#include <ui_test_expect.h>
void
ui_osx_dispatch (const struct ui_osx_t *)
{
}

void
ui_osx_update (struct ui_osx_t *)
{
}

static ui_test_case
ui_osx_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_osx_test);

  ui_test_skip ("TODO");
}
