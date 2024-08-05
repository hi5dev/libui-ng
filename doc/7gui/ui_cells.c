#if defined(WIN32)
#include <windows.h>
#define ui_cells_main(...) WINAPI wWinMain (const HINSTANCE, HINSTANCE, LPWSTR, int)
#else
#define ui_cells_main(...) main (void)
#endif

#include "ui_cells.h"

#include <ui_main.h>

int
ui_cells_main (void)
{
  return ui_main ();
}
