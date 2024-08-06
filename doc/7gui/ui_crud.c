#include "main.h"

#include "ui_crud.h"

struct ui_crud_t
{
  int unused; //!< @brief Placeholder to satisfy the non-empty struct requirement of some compilers.
};

int
ui_crud_main (struct ui_crud_t *)
{
  return ui_main ();
}

int
main (void)
{
  struct ui_crud_t crud = {};
  return ui_crud_main (&crud);
}
