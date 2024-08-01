#include <ui_test.h>
#include <ui_test_expect.h>

static void ui_test_runner
ui_headless_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_headless_test);

  ui_test_skip ("TODO");
}
