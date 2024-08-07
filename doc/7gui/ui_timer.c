#include "ui_timer.h"
#include "main.h"

#include <assert.h>
#include <ui_window.h>

struct ui_timer_t
{
  /// @brief The application's main window.
  struct ui_window_t *window;

  /// @brief Duration of the timer in seconds.
  double duration;

  /// @brief Time elapsed in seconds.
  double elapsed;
};

int
ui_timer_main (struct ui_timer_t *timer)
{
  ui_window_show (timer->window);

  const int exit_code = ui_main ();

  ui_window_destroy (timer->window);

  return exit_code;

}

int
main (void)
{
  struct ui_timer_t timer = { .duration = UI_TIMER_DEFAULT_DURATION, .elapsed = 0 };

  timer.window = ui_window_create ("Timer", 320, 240, 0);
  assert (timer.window != NULL);

  return ui_timer_main (&timer);
}
