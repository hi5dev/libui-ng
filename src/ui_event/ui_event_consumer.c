#include <ui_event_consumer.h>

#include "ui_event_error.h"
#include "ui_event_thread.h"

#include <malloc.h>

struct ui_event_consumer_t *
ui_event_consumer_create (void)
{
  struct ui_event_consumer_t *subscriber = calloc (1, sizeof (struct ui_event_consumer_t));
  if (subscriber == NULL)
    {
      ui_event_perror (errno);
      return NULL;
    }

  ui_event_thread_mutex_init (subscriber->lock, NULL);

  return subscriber;
}

void
ui_event_subscriber_destroy (struct ui_event_consumer_t *subscriber)
{
  if (subscriber == NULL)
    return;

  ui_event_thread_mutex_destroy (subscriber->lock);

  free (subscriber);
}
