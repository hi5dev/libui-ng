#pragma once

#include <stdint.h>
#include <sys/time.h>

struct ui_event_consumer_t;   //!< Receives notifications about events.
struct ui_event_dispatcher_t; //!< Queues and dispatches events to consumers.
struct ui_event_list_t;       //!< A double-linked list.
struct ui_event_producer_t;   //!< A producer is an object that can produce events.
struct ui_event_t;            //!< Information about an event.
struct ui_event_type_t;       //!< Event type - e.g. click, drag, focus, leave, etc.

typedef struct timeval ui_event_timestamp_t; //!< Event timestamps.

typedef intptr_t ui_event_data_t;   //!< Payload for events.
typedef intptr_t ui_event_mutex_t;  //!< Mutex type used by the event system.
typedef intptr_t ui_event_thread_t; //!< Thread type used by the event system.

struct ui_event_t
{
  const struct ui_event_producer_t *producer;   //!< Producer of the event.
  const struct ui_event_type_t     *event_type; //!< The event's type.
  const ui_event_data_t             data;       //!< Event payload.
  const ui_event_data_t             object;     //!< Object that caused the event.
  const ui_event_timestamp_t        timestamp;  //!< When the event was created.
};

/**
 * @brief Event callback.
 * @return non-zero to prevent any more callbacks from processing the event.
 */
typedef int (ui_event_cb) (  //!< @params
    struct ui_event_t *event //!< Event data.
);
