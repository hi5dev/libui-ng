#if defined(WIN32)
#include <windows.h>
#define ui_circle_drawer_main(...) WINAPI wWinMain (const HINSTANCE, HINSTANCE, LPWSTR, int)
#else
#define ui_circle_drawer_main(...) main (void)
#endif

#include "ui_circle_drawer.h"

#include <ui_main.h>

int
ui_circle_drawer_main (void)
{
  return ui_main ();
}
