#include <ui_test.h>
#include <ui_test_expect.h>

static ui_test_case
ui_osx_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_osx_test);

  ui_test_skip ("TODO");
}
