#include "main.h"

#include "ui_counter.h"

#include <ui_window.h>

struct ui_counter_t
{
  struct ui_window_t *window;

  int value; //!< @brief Number of times the user has clicked the button.
};

int
ui_counter_main (struct ui_counter_t *counter)
{

  const int exit_code = ui_main ();

  ui_window_destroy (counter->window);

  return exit_code;
}

int
main (void)
{
  struct ui_counter_t counter = { 0 };

  counter.window = ui_window_create ("Counter", 320, 200, 0);
  if (!counter.window)
    return EXIT_FAILURE;

  return ui_counter_main (&counter);
}
