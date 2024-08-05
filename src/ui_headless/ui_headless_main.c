#include <ui_headless.h>
#include <ui_headless_main.h>
#include <ui_main.h>

#include <ui_test.h>
#include <ui_test_expect.h>

#include <stddef.h>

static void
ui_headless_main_dispatch (struct ui_main_t *ui_main)
{
  if (ui_main == NULL || ui_main->data == NULL)
    return;

  ui_headless_dispatch (ui_main->data);
}

static void
ui_headless_main_update (struct ui_main_t *ui_main)
{
  if (ui_main == NULL || ui_main->data == NULL)
    return;

  struct ui_headless_t *ui_headless = ui_main->data;

  ui_headless_update (ui_headless);

  if (ui_headless->quit)
    ui_main_quit (ui_main);
}

int
ui_headless_main (struct ui_headless_t *ui_headless)
{
  struct ui_main_t ui_main = {
    .exit_code = 0,
    .running   = 0,
    .data      = ui_headless,
    .dispatch  = ui_headless_main_dispatch,
    .update    = ui_headless_main_update,
  };

  ui_main_run (&ui_main);

  return ui_main.exit_code;
}

static ui_test_case
ui_headless_main_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_headless_main_test);

  ui_test_skip ("TODO");
}
