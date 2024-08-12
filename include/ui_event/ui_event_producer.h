#pragma once

#include "ui_event_consumer.h"

#include <pthread.h>

/**
 * @brief A producer is an object that can dispatch events - e.g. a window or button.
 */
struct ui_event_producer_t
{
  /// @brief Mutually-exclusive lock used when adding or removing event producers.
  pthread_mutex_t lock;

  /// @brief The producer's unique identifier.
  unsigned long id;

  /// @brief List of subscribers to the producer's events.
  struct ui_event_consumer_t *consumers;

  /// @brief Previous producer.
  struct ui_event_producer_t *previous;

  /// @brief Next producer.
  struct ui_event_producer_t *next;
};
