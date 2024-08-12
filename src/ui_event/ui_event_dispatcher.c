#include <ui_event_dispatcher.h>

#include "ui_event_error.h"
#include "ui_event_thread.h"

#include <assert.h>
#include <malloc.h>
#include <stdio.h>

struct ui_event_dispatcher_t *
ui_event_dispatcher_create (void)
{
  struct ui_event_dispatcher_t *dispatcher = calloc (1, sizeof (struct ui_event_dispatcher_t));
  if (dispatcher == NULL)
    {
      ui_event_perror (errno);
      return NULL;
    }

  ui_event_thread_mutex_init (dispatcher->lock, NULL);

  return dispatcher;
}

void
ui_event_dispatcher_destroy (struct ui_event_dispatcher_t *dispatcher)
{
  if (dispatcher == NULL)
    return;

  ui_event_thread_mutex_destroy (dispatcher->lock);

  free (dispatcher);
}

void
ui_event_dispatch (struct ui_event_dispatcher_t *dispatcher, const unsigned long id, const intptr_t data)
{
  assert (dispatcher != NULL);

  struct ui_event_t event = ui_event_init (id, data);

  pthread_mutex_lock (&dispatcher->lock);

  for (struct ui_event_producer_t *producer = dispatcher->producers; producer != NULL; producer = producer->next)
    {
      pthread_mutex_lock (&producer->lock);

      for (const struct ui_event_consumer_t *sub = producer->subscribers; sub != NULL; sub = sub->next)
        {
          if (sub->callback != NULL)
            {
              if (sub->callback (&event, event.data))
                break;
            }
        }

      pthread_mutex_unlock (&producer->lock);
    }

  pthread_mutex_unlock (&dispatcher->lock);
}
