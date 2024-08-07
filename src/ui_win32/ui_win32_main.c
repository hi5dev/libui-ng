#include <assert.h>
#include <windows.h>

#include <ui_main.h>
#include <ui_win32.h>

#include <ui_test.h>
#include <ui_test_expect.h>

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
ui_main (void)
{
  const HINSTANCE hInstance = GetModuleHandle (NULL);
  if (hInstance == NULL)
    ui_win32_log_last_error ("GetModuleHandle");

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

static ui_test_case
ui_win32_main_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_win32_main_test);

  ui_test_skip ("TODO");
}
