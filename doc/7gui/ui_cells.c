#include "ui_cells.h"
#include "main.h"

#include <assert.h>
#include <tinyexpr.h>
#include <ui_window.h>

struct ui_cells_t
{
  /// @brief The application's main window.
  struct ui_window_t *window;
};

int
ui_cells_main (struct ui_cells_t *cells)
{
  ui_window_show (cells->window);

  const int exit_code = ui_main ();

  ui_window_destroy (cells->window);

  return exit_code;
}

int
main (void)
{
  struct ui_cells_t cells = { 0 };

  cells.window = ui_window_create ("Cells", 320, 240, 0);
  assert (cells.window != NULL);

  return ui_cells_main (&cells);
}

double
ui_cells_evaluate (const char *formula, int *error)
{
  return te_interp (formula, error);
}
