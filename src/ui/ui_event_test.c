#include <ui_event.h>
#include <ui_test.h>

static void ui_test_constructor
ui_event_test ()
{
  static struct ui_test_t test = ui_test (ui_event_test);

  ui_test_register (test);
}
