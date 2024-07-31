#include <windows.h>

#include <ui_win32.h>
#include <ui_win32_main.h>
#include <ui_main.h>

static void
ui_win32_main_dispatch (struct ui_main_t *main)
{
  if (main->data != NULL)
    ui_win32_dispatch (main->data);
}

static void
ui_win32_main_update (struct ui_main_t *ui_main)
{
  struct ui_win32_t *ui_win32 = ui_main->data;

  if (ui_win32 == NULL)
    return;

  ui_win32_update (ui_win32);

  if (ui_win32->quit)
    {
      ui_main->exit_code = ui_win32->message.wParam;
      ui_main_quit (ui_main);
    }
}

int
ui_win32_main (struct ui_win32_t *ui_win32)
{
  struct ui_main_t ui_main = {
    .exit_code = 0,
    .running   = 0,
    .data      = ui_win32,
    .dispatch  = ui_win32_main_dispatch,
    .update    = ui_win32_main_update,
  };

  ui_main_run (&ui_main);

  return ui_main.exit_code;
}

int WINAPI
wWinMain (const HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
  UNREFERENCED_PARAMETER (hPrevInstance);
  UNREFERENCED_PARAMETER (lpCmdLine);
  UNREFERENCED_PARAMETER (nShowCmd);

  struct ui_win32_t win32 = { .handle = hInstance, .message = {}, .quit = FALSE };

  return ui_win32_main (&win32);
}
