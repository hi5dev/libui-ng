#if defined(WIN32)
#include <windows.h>
#define ui_crud_main(...) WINAPI wWinMain (const HINSTANCE, HINSTANCE, LPWSTR, int)
#else
#define ui_crud_main(...) main (void)
#endif

#include "ui_crud.h"

#include <ui_main.h>

int
ui_crud_main (void)
{
  return ui_main ();
}
