#include <ui_main.h>

#include <ui_test.h>
#include <ui_test_expect.h>

#include <stddef.h>

void
ui_main_run (struct ui_main_t *ui_main)
{
  if (ui_main == NULL || ui_main->dispatch == NULL || ui_main->update == NULL)
    return;

  if (ui_main->running != 0)
    return;

  ui_main->running = 1;

  while (ui_main->running)
    {
      ui_main_update (ui_main);
      ui_main_dispatch (ui_main);
    }
}

void
ui_main_quit (struct ui_main_t *ui_main)
{
  if (ui_main != NULL)
    ui_main->running = 0;
}

void
ui_main_dispatch (struct ui_main_t *ui_main)
{
  if (ui_main != NULL && ui_main->dispatch != NULL)
    ui_main->dispatch (ui_main);
}

void
ui_main_update (struct ui_main_t *ui_main)
{
  if (ui_main != NULL && ui_main->update != NULL)
    ui_main->update (ui_main);
}

static ui_test_case
ui_main_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_main_test);

  ui_test_skip ("TODO");
}
