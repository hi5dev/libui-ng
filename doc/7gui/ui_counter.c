#if defined(WIN32)
#include <windows.h>
#define ui_counter_main(...) WINAPI wWinMain (const HINSTANCE, HINSTANCE, LPWSTR, int)
#else
#define ui_counter_main(...) main (void)
#endif

#include "ui_counter.h"

#include <ui_main.h>

/**
 * @implements ui_counter_t
 */
struct ui_counter_p
{
  /**
   * @brief The counter's current value.
   */
  int value;
};

int
ui_counter_main (void)
{
  // struct ui_counter_t *counter = ui_counter_create ();
  //
  // struct ui_counter_p counter_data = { 0 };
  //
  // counter->impl = &counter_data;
  //
  // ui_counter_destroy (counter);

  return ui_main ();
}
