#include <windows.h>

#include <ui_main.h>
#include <ui_win32.h>
#include <ui_win32_error.h>

#include <assert.h>

static void
ui_win32_main_dispatch (struct ui_main_t *main)
{
  assert (main != NULL);

  ui_win32_dispatch (main->data);
}

static void
ui_win32_main_update (struct ui_main_t *ui_main)
{
  assert (ui_main != NULL);

  struct ui_win32_t *ui_win32 = ui_main->data;

  ui_win32_update (ui_win32);

  if (!ui_win32->quit)
    return;

  ui_main->exit_code = ui_win32->message.wParam;

  ui_main_quit (ui_main);
}

int
ui_main (void)
{
  const HINSTANCE hInstance = GetModuleHandle (NULL);

  if (hInstance == NULL)
    ui_win32_abort_on_error ("GetModuleHandle");

  // ask the system to send errors to the calling thread instead of displaying a critical-error-handler message box
  if (SetThreadErrorMode (SEM_FAILCRITICALERRORS, NULL) != 0)
    (void)ui_win32_log_last_error ("SetThreadErrorMode");

  struct ui_win32_t ui_win32 = { .handle = hInstance, .message = {}, .quit = FALSE };

  struct ui_main_t ui_main = {
    .exit_code = 0,
    .running   = 0,
    .data      = &ui_win32,
    .dispatch  = ui_win32_main_dispatch,
    .update    = ui_win32_main_update,
  };

  ui_main_run (&ui_main);

  return ui_main.exit_code;
}
