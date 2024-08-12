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
 * @brief Prints reason for the given @p errno to @p UI_EVENT_PERROR_STREAM.
 * @param code @p errno
 * @param stream to write the error - uses @p UI_EVENT_PERROR_STREAM, which defaults to stderr.
 * @param file with the source of the error.
 * @param func that raised the error.
 * @param line number of the error.
 */
void _ui_event_perror (int code, FILE *stream, const char *file, const char *func, int line);
