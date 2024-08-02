#include <windows.h>

#include <ui_win32.h>

#include <ui_test.h>
#include <ui_test_expect.h>

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

static ui_test_case
ui_win32_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_test);

  ui_test_skip ("TODO");
}
