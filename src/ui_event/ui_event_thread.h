#pragma once

#include <pthread.h>

/**
 * @brief @p pthread_mutex_t constructor.
 * @param mutex @p pthread_mutex_t
 * @param attr @p pthread_mutexattr_t
 * @remark Errors will be printed to stderr.
 */
void ui_event_thread_mutex_init (pthread_mutex_t mutex, const pthread_mutexattr_t *attr);

/**
 * @brief @p pthread_mutex_t destructor.
 * @param mutex @p pthread_mutex_t
 * @remark Errors will be printed to stderr.
 */
void ui_event_thread_mutex_destroy (pthread_mutex_t mutex);

/**
 * @brief Locks a mutex.
 * @param mutex @p pthread_mutex_t
 * @remark Errors will be printed to stderr.
 */
void ui_event_thread_mutex_lock (pthread_mutex_t mutex);

/**
 * @brief Unlocks a mutex.
 * @param mutex @p pthread_mutex_t
 * @remark Errors will be printed to stderr.
 */
void ui_event_thread_mutex_unlock (pthread_mutex_t mutex);
