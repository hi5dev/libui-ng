#include <ui_linux.h>
#include <ui_main.h>

#include <ui_test.h>
#include <ui_test_expect.h>

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

static ui_test_case
ui_linux_main_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_linux_main_test);

  ui_test_skip ("TODO");
}

int
ui_main (void)
{
  struct ui_linux_t ui_linux = { 0 };

  struct ui_main_t ui_main = {
    .exit_code = 0,
    .running   = 0,
    .data      = &ui_linux,
    .dispatch  = ui_linux_main_dispatch,
    .update    = ui_linux_main_update,
  };

  ui_main_run (&ui_main);

  return ui_main.exit_code;
}
