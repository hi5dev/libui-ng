#include "ui_flight_booker.h"
#include "main.h"

#include <assert.h>
#include <ui_window.h>

struct ui_flight_booker_t
{
  /// @brief The application's main window.
  struct ui_window_t *window;
};

int
ui_flight_booker_main (struct ui_flight_booker_t *flight_booker)
{
  ui_window_show (flight_booker->window);

  const int exit_code = ui_main ();

  ui_window_destroy (flight_booker->window);

  return exit_code;
}

int
main (void)
{
  struct ui_flight_booker_t flight_booker = { 0 };

  flight_booker.window = ui_window_create ("Flight Booker", 320, 240, 0);
  assert (flight_booker.window != NULL);

  return ui_flight_booker_main (&flight_booker);
}
