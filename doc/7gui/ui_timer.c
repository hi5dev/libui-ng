#include "main.h"
#include "ui_timer.h"

struct ui_timer_t
{
  double duration; //!< @brief Duration of the timer in seconds.
  double elapsed;  //!< @brief Time elapsed in seconds.
};

int
ui_timer_main (struct ui_timer_t *)
{
  return ui_main ();
}

int
main (void)
{
  struct ui_timer_t timer = { .duration = UI_TIMER_DEFAULT_DURATION, .elapsed = 0 };
  return ui_timer_main (&timer);
}
