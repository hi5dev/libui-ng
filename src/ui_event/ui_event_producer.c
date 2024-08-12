#include <ui_event_producer.h>

#include "ui_event_error.h"
#include "ui_event_thread.h"

#include <malloc.h>

struct ui_event_producer_t *
ui_event_producer_create (void)
{
  struct ui_event_producer_t *producer = calloc (1, sizeof (struct ui_event_producer_t));
  if (producer == NULL)
    {
      ui_event_perror (errno);
      return NULL;
    }

  ui_event_thread_mutex_init (producer->lock, NULL);

  return producer;
}

void
ui_event_producer_destroy (struct ui_event_producer_t *producer)
{
  if (producer == NULL)
    return;

  ui_event_thread_mutex_destroy (producer->lock);

  free (producer);
}
