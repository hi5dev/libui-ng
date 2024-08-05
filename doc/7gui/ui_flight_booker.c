#if defined(WIN32)
#include <windows.h>
#define ui_flight_booker_main(...) WINAPI wWinMain (const HINSTANCE, HINSTANCE, LPWSTR, int)
#else
#define ui_flight_booker_main(...) main (void)
#endif

#include "ui_flight_booker.h"

#include <ui_main.h>

int
ui_flight_booker_main (void)
{
  return ui_main ();
}
