#pragma once

#include "ui_event.h"

//! @see @file ui_event.h
struct ui_event_type_t
{
  const char *id; //!< Unique identifier.
};

/**
 * @brief Creates an event type.
 * @param N A unique and valid C variable name.
 */
#define ui_event_type(N) struct ui_event_type_t N = { .id = #N }
