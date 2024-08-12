#pragma once

#include "ui_event.h"

#include <pthread.h>

/**
 * @brief A consumer receives notifications about events dispatched by a producer.
 */
struct ui_event_consumer_t
{
  /// @brief Mutually-exclussive lock used while adding or removing subscribers.
  pthread_mutex_t lock;

  /// @brief ID of the type of event the subscriber is interested in.
  unsigned long id;

  /// @brief Event callback function.
  ui_event_cb *callback;

  /// @brief Previous subscriber.
  struct ui_event_consumer_t *previous;

  /// @brief Next subscriber.
  struct ui_event_consumer_t *next;
};

/**
 * @brief @p ui_event_subscriber_t constructor.
 * @param dispatcher @p ui_event_dispatcher_t
 * @return @p ui_event_subscriber_t
 */
struct ui_event_consumer_t *ui_event_consumer_create (struct ui_event_dispatcher_t *dispatcher);

/**
 * @brief @p ui_event_subscriber_t destructor.
 * @param consumer @p ui_event_subscriber_t
 */
void ui_event_consumer_destroy (struct ui_event_consumer_t *consumer);
