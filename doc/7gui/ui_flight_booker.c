#include "main.h"

#include "ui_flight_booker.h"

struct ui_flight_booker_t
{
  int unused; //!< @brief Placeholder to satisfy the non-empty struct requirement of some compilers.
};

int
ui_flight_booker_main (struct ui_flight_booker_t *)
{
  return ui_main ();
}

int
main (void)
{
  struct ui_flight_booker_t flight_booker = { 0 };
  return ui_flight_booker_main (&flight_booker);
}
