#include <ui_control.h>
#include <ui_test.h>
#include <ui_test_expect.h>

#include <malloc.h>

struct ui_control_t
{
  int unused;
};

struct ui_control_t *
ui_control_create (void)
{
  return calloc (1, sizeof (struct ui_control_t));
}

static ui_test_case
ui_control_test_create (void)
{
  static struct ui_test_t test = ui_test (test, ui_control_test_create);

  struct ui_control_t *control = ui_control_create ();
  ui_expect_not_null (control);
  ui_control_destroy (control);
}

void
ui_control_destroy (struct ui_control_t *ui_control)
{
  free (ui_control);
}
