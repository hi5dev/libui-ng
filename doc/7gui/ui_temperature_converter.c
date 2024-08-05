#if defined(WIN32)
#include <windows.h>
#define ui_temperature_converter_main(...) WINAPI wWinMain (const HINSTANCE, HINSTANCE, LPWSTR, int)
#else
#define ui_temperature_converter_main(...) main (void)
#endif

#include "ui_temperature_converter.h"

#include <ui_main.h>

int
ui_temperature_converter_main (void)
{
  return ui_main ();
}
