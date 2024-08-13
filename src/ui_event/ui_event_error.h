#pragma once

#include <stdio.h>

#ifndef UI_EVENT_PERROR_STREAM
/**
 * @brief Stream to write error messages caused by ui_event.
 * @details Set before including this file to write to a different FILE stream.
 */
#define UI_EVENT_PERROR_STREAM stderr
#endif

/**
 * @brief Prints the reason for the given @p errno to @p UI_EVENT_PERROR_STREAM.
 * @param code @p errno
 */
#define ui_event_perror(code) _ui_event_perror (code, UI_EVENT_PERROR_STREAM, __FILE__, __func__, __LINE__)

/**
 * @brief Prints the reason for the current @p errno to @p UI_EVENT_PERROR_STREAM.
 */
#define ui_event_perrno() _ui_event_perror (errno, UI_EVENT_PERROR_STREAM, __FILE__, __func__, __LINE__)

//! @brief Prints reason for the given @p errno to @p UI_EVENT_PERROR_STREAM.
void _ui_event_perror ( //!< @params
    int         code,   //!< Error number.
    FILE       *stream, //!< File stream to write the message.
    const char *file,   //!< Full path to the source file.
    const char *func,   //!< Name of the source function.
    int         line    //!< Line number with the code that caused the error.
);
