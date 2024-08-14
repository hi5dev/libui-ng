#pragma once

#include "ui_event.h"

//! @see @file ui_event.h
struct ui_event_consumer_t
{
  ui_event_cb    *callback; //!< The consumer's event callback.
  ui_event_data_t data;     //!< User-defined data to send to the callback.
  ui_event_data_t object;   //!< Object the consumer is interested in.
};

//!< @brief @p ui_event_consumer_t constructor.
//!< @returns @p ui_event_consumer_t
struct ui_event_consumer_t *ui_event_consumer_create ( //!< @params
    ui_event_cb    *callback,                          //!< Callback function.
    ui_event_data_t data,                              //!< User-defined data.
    ui_event_data_t object                             //!< Object the consumer is interested in.
);

//!< @brief @p ui_event_consumer_t destructor.
void ui_event_consumer_destroy (         //!< @params
    struct ui_event_consumer_t *consumer //!< The consumer to destroy.
);
