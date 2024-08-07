#include "ui_crud.h"
#include "main.h"

#include <assert.h>
#include <ui_window.h>

struct ui_crud_t
{
  /// @brief The application's main window.
  struct ui_window_t *window;
};

int
ui_crud_main (struct ui_crud_t *crud)
{
  ui_window_show (crud->window);

  const int exit_code = ui_main ();

  ui_window_destroy (crud->window);

  return exit_code;
}

int
main (void)
{
  struct ui_crud_t crud = {};

  crud.window = ui_window_create ("CRUD", 320, 240, 0);
  assert (crud.window != NULL);

  return ui_crud_main (&crud);
}
