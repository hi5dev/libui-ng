#include <ui_main.h>
#include <ui_osx.h>
#include <ui_osx_main.h>
#include <ui_test.h>
#include <ui_test_expect.h>

#include <stddef.h>

static void
ui_osx_main_dispatch (struct ui_main_t *ui_main)
{
  if (ui_main == NULL || ui_main->data == NULL)
    return;

  ui_osx_dispatch (ui_main->data);
}

static void
ui_osx_main_update (struct ui_main_t *ui_main)
{
  if (ui_main == NULL || ui_main->data == NULL)
    return;

  struct ui_osx_t *ui_osx = ui_main->data;

  ui_osx_update (ui_osx);

  if (ui_osx->quit)
    ui_main_quit (ui_main);
}

int
ui_osx_main (struct ui_osx_t *ui_osx)
{
  struct ui_main_t ui_main = {
    .exit_code = 0,
    .running   = 0,
    .data      = ui_osx,
    .dispatch  = ui_osx_main_dispatch,
    .update    = ui_osx_main_update,
  };

  ui_main_run (&ui_main);

  return ui_main.exit_code;
}

static ui_test_case
ui_osx_main_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_osx_main_test);

  ui_test_skip ("TODO");
}
