#include <ui_control.h>

#include <malloc.h>
#
struct ui_control_t
{
  int unused;
};

struct ui_control_t *
ui_control_create (void)
{
  return calloc (1, sizeof (struct ui_control_t));
}

void
ui_control_destroy (struct ui_control_t *ui_control)
{
  free (ui_control);
}
