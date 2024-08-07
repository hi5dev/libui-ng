#include "ui_counter.h"
#include "main.h"

#include <assert.h>
#include <ui_window.h>

struct ui_counter_t
{
  /// @brief The application's main window.
  struct ui_window_t *window;

  /// @brief Number of times the user has clicked the button.
  int value;
};

int
ui_counter_main (struct ui_counter_t *counter)
{
  ui_window_show (counter->window);

  const int exit_code = ui_main ();

  ui_window_destroy (counter->window);

  return exit_code;
}

int
main (void)
{
  struct ui_counter_t counter = { 0 };

  counter.window = ui_window_create ("Counter", 320, 240, 0);
  assert (counter.window != NULL);

  return ui_counter_main (&counter);
}
