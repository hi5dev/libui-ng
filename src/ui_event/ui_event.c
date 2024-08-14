#include <ui_event.h>
#include <ui_test_expect.h>

#include "ui_event_error.h"

#include <stdlib.h>

struct ui_event_t *
ui_event_create (void)
{
  struct ui_event_t *event = calloc (1, sizeof (*event));

  if (event == NULL)
    ui_event_perrno ();

  return event;
}

void
ui_event_destroy (struct ui_event_t *event)
{
  free (event);
}

static ui_test_case
ui_event_cdtor_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_cdtor_test);

  struct ui_event_t *event = ui_event_create ();

  ui_expect_not_null (event);

  ui_expect_null (event->data);
  ui_expect_null (event->event_type);
  ui_expect_null (event->object);
  ui_expect_null (event->producer);

  ui_expect_cmp (int, event->timestamp.tv_sec, is, 0);
  ui_expect_cmp (int, event->timestamp.tv_usec, is, 0);

  ui_event_destroy (event);
}
