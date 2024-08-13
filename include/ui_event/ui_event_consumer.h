#pragma once

#include "ui_event.h"

//! @see @file ui_event.h
struct ui_event_consumer_t
{
  ui_event_cb *callback; //!< The consumer's event callback.
  const void  *data;     //!< User-defined data to send to the callback.
  const void  *object;   //!< Object the consumer is interested in.
};

//!< @brief @p ui_event_consumer_t constructor.
//!< @returns @p ui_event_consumer_t
struct ui_event_consumer_t *ui_event_consumer_create ( //!< @params
    ui_event_cb *callback,                             //!< Callback function.
    void        *data,                                 //!< User-defined data.
    void        *object                                //!< Object the consumer is interested in.
);

//!< @brief @p ui_event_consumer_t destructor.
void ui_event_consumer_destroy (         //!< @params
    struct ui_event_consumer_t *consumer //!< The consumer to destroy.
);
