#include <ui_test.h>
#include <ui_test_expect.h>

static void ui_test_runner
ui_osx_main_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_osx_main_test);

  ui_test_skip ("TODO");
}
