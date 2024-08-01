#include <ui_test.h>
#include <ui_test_expect.h>

#include <ui_control.h>

#include <stddef.h>

static void ui_test_runner
ui_control_test_ctor (void)
{
  static struct ui_test_t test = ui_test (test, ui_control_test_ctor);

  struct ui_control_t *control = ui_control_create ();
  ui_expect (control != NULL, "control was not constructed");
  ui_control_destroy (control);
}
