#include "ui_circle_drawer.h"
#include "main.h"

#include <assert.h>
#include <ui_window.h>

struct ui_circle_drawer_t
{
  /// @brief The application's main window.
  struct ui_window_t *window;
};

int
ui_circle_drawer_main (struct ui_circle_drawer_t *circle_drawer)
{
  ui_window_show (circle_drawer->window);

  const int exit_code = ui_main ();

  ui_window_destroy (circle_drawer->window);

  return exit_code;
}

int
main (void)
{
  struct ui_circle_drawer_t circle_drawer = {};

  circle_drawer.window = ui_window_create ("Circle Drawer", 320, 240, 0);
  assert (circle_drawer.window != NULL);

  return ui_circle_drawer_main (&circle_drawer);
}
