#include "main.h"

#include "ui_circle_drawer.h"

struct ui_circle_drawer_t
{
  int unused; //!< @brief Placeholder to satisfy the non-empty struct requirement of some compilers.
};

int
ui_circle_drawer_main (struct ui_circle_drawer_t *)
{
  return ui_main ();
}

int
main (void)
{
  struct ui_circle_drawer_t circle_drawer = {};
  return ui_circle_drawer_main (&circle_drawer);
}
