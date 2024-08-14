#include <ui_event_consumer.h>
#include <ui_event_dispatcher.h>
#include <ui_event_list.h>
#include <ui_test_expect.h>

#include "ui_event_error.h"
#include "ui_event_producer.h"

#include <assert.h>
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
    ui_event_consumer_destroy (i->consumer);

  ui_event_list_clear (dispatcher->consumers);
  ui_event_list_clear (dispatcher->events);
  ui_event_list_clear (dispatcher->producers);

  free (dispatcher);
}

void
ui_event_dispatch (struct ui_event_dispatcher_t *dispatcher)
{
}

//! @brief Ensures the given producer exists in the dispatcher; adds it if not.
static void
ui_event_dispatcher_add_producer_once (       //!< @params
    struct ui_event_dispatcher_t *dispatcher, //!< The dispatcher.
    struct ui_event_producer_t   *producer    //!< The producer to ensure is present.
)
{
  struct ui_event_list_t *producers = ui_event_list_first (dispatcher->producers);

  for (const struct ui_event_list_t *i = producers; i != NULL; i = i->next)
    if (i->producer == producer)
      return;

  dispatcher->producers = ui_event_list_insert (producers, producer);
}

static ui_test_case
ui_event_dispatcher_add_producer_once_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_dispatcher_add_producer_once_test);

  struct ui_event_producer_t producer1 = { .id = "ui_event_dispatcher_add_producer_once_test_producer1" };
  struct ui_event_producer_t producer2 = { .id = "ui_event_dispatcher_add_producer_once_test_producer2" };

  struct ui_event_dispatcher_t *dispatcher = ui_event_dispatcher_create ();

  const int initial_count = ui_event_list_size (dispatcher->producers);

  ui_event_dispatcher_add_producer_once (dispatcher, &producer1);
  const int count_after_adding_producer1_once = ui_event_list_size (dispatcher->producers);

  ui_event_dispatcher_add_producer_once (dispatcher, &producer1);
  const int count_after_adding_producer1_again = ui_event_list_size (dispatcher->producers);

  ui_event_dispatcher_add_producer_once (dispatcher, &producer2);
  const int count_after_adding_producer2_once = ui_event_list_size (dispatcher->producers);

  ui_event_dispatcher_add_producer_once (dispatcher, &producer2);
  const int count_after_adding_producer2_again = ui_event_list_size (dispatcher->producers);

  ui_event_dispatcher_destroy (dispatcher);

  ui_expect_cmp (int, initial_count, is, 0);
  ui_expect_cmp (int, count_after_adding_producer1_once, is, 1);
  ui_expect_cmp (int, count_after_adding_producer1_again, is, 1);
  ui_expect_cmp (int, count_after_adding_producer2_once, is, 2);
  ui_expect_cmp (int, count_after_adding_producer2_again, is, 2);
}

void
ui_event_dispatcher_subscribe (struct ui_event_dispatcher_t *dispatcher, struct ui_event_producer_t *producer,
                               ui_event_cb *callback, const ui_event_data_t data, const ui_event_data_t object)
{
  assert (dispatcher != NULL);
  assert (producer != NULL);
  assert (callback != NULL);

  ui_event_dispatcher_add_producer_once (dispatcher, producer);

  struct ui_event_consumer_t *consumer = ui_event_consumer_create (callback, data, object);

  dispatcher->consumers = ui_event_list_insert (dispatcher->consumers, consumer);
}
