#include "ui_event_consumer.h"

#include <ui_event_dispatcher.h>

#include "ui_event_error.h"
#include "ui_event_list.h"

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
  if (__builtin_expect (dispatcher == NULL, 0))
    return;

  ui_event_list_clear (dispatcher->consumers);
  ui_event_list_clear (dispatcher->events);
  ui_event_list_clear (dispatcher->producers);

  free (dispatcher);
}

void
ui_event_dispatch (struct ui_event_dispatcher_t *dispatcher)
{
}
