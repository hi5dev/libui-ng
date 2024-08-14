#include <ui_event_consumer.h>
#include <ui_event_dispatcher.h>
#include <ui_event_list.h>
#include <ui_test_expect.h>

#include "ui_event_error.h"

#include <stdlib.h>

struct ui_event_dispatcher_t *
ui_event_dispatcher_create (void)
{
  struct ui_event_dispatcher_t *dispatcher = calloc (1, sizeof (*dispatcher));

  if (dispatcher == NULL)
    ui_event_perrno ();

  return dispatcher;
}

void
ui_event_dispatcher_destroy (struct ui_event_dispatcher_t *dispatcher)
{
  if (dispatcher == NULL)
    return;

  for (const struct ui_event_list_t *i = ui_event_list_first (dispatcher->consumers); i != NULL; i = i->next)
    ui_event_consumer_destroy ((struct ui_event_consumer_t *)i->item);

  ui_event_list_clear (dispatcher->consumers);
  ui_event_list_clear (dispatcher->events);
  ui_event_list_clear (dispatcher->producers);

  free (dispatcher);
}

static ui_test_case
ui_event_dispatcher_cdtor_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_dispatcher_cdtor_test);

  struct ui_event_dispatcher_t *dispatcher = ui_event_dispatcher_create ();

  ui_expect_null(dispatcher->consumers);
  ui_expect_null(dispatcher->events);
  ui_expect_null(dispatcher->producers);

  ui_event_dispatcher_destroy (dispatcher);
}

void
ui_event_dispatch (struct ui_event_dispatcher_t *dispatcher)
{
}
