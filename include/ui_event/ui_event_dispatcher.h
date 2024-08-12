#pragma once

#include "ui_event_producer.h"

#include <pthread.h>

/**
 * @brief Dispatchers notify all consumers of events from the producers on a scheduled inverval.
 */
struct ui_event_dispatcher_t
{
  /**
   * @brief Thread from which the events are dispatched.
   */
  pthread_t thread;

  /**
   * @brief Mutually-exclusive lock used when dispatching events.
   */
  pthread_mutex_t lock;

  /**
   * @brief List of event producers.
   */
  struct ui_event_producer_t *producers;
};

/**
 * @brief @p ui_event_dispatcher_t constructor.
 * @return @p ui_event_dispatcher_t
 */
struct ui_event_dispatcher_t *ui_event_dispatcher_create (void);

/**
 * @brief @p ui_event_dispatcher_t destructor.
 * @param dispatcher @p ui_event_dispatcher_t
 */
void ui_event_dispatcher_destroy (struct ui_event_dispatcher_t *dispatcher);

/**
 * @brief Dispatches an event.
 * @param dispatcher @p ui_event_dispatcher_t
 * @param id of the event type.
 * @param data to send to the event callbacks.
 */
void ui_event_dispatch (struct ui_event_dispatcher_t *dispatcher, unsigned long id, intptr_t data);
