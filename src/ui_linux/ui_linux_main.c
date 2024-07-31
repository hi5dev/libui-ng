#include <ui_linux.h>
#include <ui_linux_main.h>
#include <ui_main.h>

#include <stddef.h>

static void
ui_linux_main_dispatch (struct ui_main_t *ui_main)
{
  if (ui_main == NULL || ui_main->data == NULL)
    return;

  ui_linux_dispatch (ui_main->data);
}

static void
ui_linux_main_update (struct ui_main_t *ui_main)
{
  if (ui_main == NULL || ui_main->data == NULL)
    return;

  struct ui_linux_t *ui_linux = ui_main->data;

  ui_linux_update (ui_linux);

  if (ui_linux->quit)
    ui_main_quit (ui_main);
}

int
ui_linux_main (struct ui_linux_t *ui_linux)
{
  struct ui_main_t ui_main = {
    .exit_code = 0,
    .running   = 0,
    .data      = ui_linux,
    .dispatch  = ui_linux_main_dispatch,
    .update    = ui_linux_main_update,
  };

  ui_main_run (&ui_main);

  return ui_main.exit_code;
}

int
main (void)
{
  struct ui_linux_t win32 = { .quit = 0 };
  return ui_linux_main (&win32);
}
