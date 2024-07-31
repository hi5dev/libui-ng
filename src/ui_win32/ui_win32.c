#include <windows.h>

#include <ui_win32.h>

void
ui_win32_dispatch (struct ui_win32_t *ui_win32)
{
  TranslateMessage (&ui_win32->message);

  DispatchMessage (&ui_win32->message);
}

void
ui_win32_update (struct ui_win32_t *ui_win32)
{
  if (GetMessage (&ui_win32->message, NULL, 0, 0) != 0)
    ui_win32->quit = TRUE;
}
