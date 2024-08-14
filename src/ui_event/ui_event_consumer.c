#include <ui_event_consumer.h>

#include "ui_event_error.h"

#include <stdlib.h>

struct ui_event_consumer_t *
ui_event_consumer_create (ui_event_cb *callback, const ui_event_data_t data, const ui_event_data_t object)
{
  struct ui_event_consumer_t *consumer = calloc (1, sizeof (*consumer));

  if (consumer == NULL)
    {
      ui_event_perrno ();
      return NULL;
    }

  consumer->callback = callback;
  consumer->data     = data;
  consumer->object   = object;

  return consumer;
}

void
ui_event_consumer_destroy (struct ui_event_consumer_t *consumer)
{
  free (consumer);
}
