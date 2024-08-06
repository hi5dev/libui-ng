#include "main.h"

#include "ui_cells.h"

#include <tinyexpr.h>

struct ui_cells_t
{
  int unused; //!< @brief Placeholder to satisfy the non-empty struct requirement of some compilers.
};

int
ui_cells_main (struct ui_cells_t *)
{
  return ui_main ();
}

int
main (void)
{
  struct ui_cells_t cells = { 0 };
  return ui_cells_main (&cells);
}

double
ui_cells_evaluate (const char *formula, int *error)
{
  return te_interp (formula, error);
}
