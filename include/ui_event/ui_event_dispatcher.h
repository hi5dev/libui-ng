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
