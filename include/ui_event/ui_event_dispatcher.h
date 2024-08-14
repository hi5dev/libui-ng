#pragma once

#include "ui_event.h"

//! @see @file ui_event.h
struct ui_event_dispatcher_t
{
  struct ui_event_list_t *consumers; //!< List of event consumers.
  struct ui_event_list_t *events;    //!< List of queued events.
  struct ui_event_list_t *producers; //!< List of event producers.
};

//! @brief @p ui_event_dispatcher_t constructor.
//! @return @p ui_event_dispatcher_t
struct ui_event_dispatcher_t *ui_event_dispatcher_create (void);

//! @brief @p ui_event_dispatcher_t destructor.
void ui_event_dispatcher_destroy (           //!< @params
    struct ui_event_dispatcher_t *dispatcher //!< The dispatcher.
);

//! @brief Dispatch all queued events.
void ui_event_dispatch (                     //!< @params
    struct ui_event_dispatcher_t *dispatcher //!< The dispatcher
);

//! @brief Listens to events that fit the given criteria.
void ui_event_dispatcher_subscribe (          //!< @params
    struct ui_event_dispatcher_t *dispatcher, //!< The event dispatcher.
    struct ui_event_producer_t   *producer,   //!< The event producer the consumer is interested in.
    ui_event_cb                  *callback,   //!< Event notification callback.
    ui_event_data_t               data,       //!< Payload for the callback.
    ui_event_data_t               object      //!< A specific object the consumer is interested in or @p NULL for any.
);
