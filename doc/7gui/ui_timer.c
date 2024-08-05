#if defined(WIN32)
#include <windows.h>
#define ui_timer_main(...) WINAPI wWinMain (const HINSTANCE, HINSTANCE, LPWSTR, int)
#else
#define ui_timer_main(...) main (void)
#endif

#include "ui_timer.h"

#include <ui_main.h>

int
ui_timer_main (void)
{
  struct ui_timer_t timer = { 50, 0 };

  return ui_main ();
}
