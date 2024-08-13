#include "ui_event_thread.h"
#include "ui_event_error.h"

void
ui_event_thread_mutex_init (pthread_mutex_t mutex, const pthread_mutexattr_t *attr)
{
  const int err = pthread_mutex_init (&mutex, attr);

  if (__builtin_expect (err != 0, 0))
    ui_event_perror (err);
}

void
ui_event_thread_mutex_destroy (pthread_mutex_t mutex)
{
  const int err = pthread_mutex_destroy (&mutex);

  if (__builtin_expect (err != 0, 0))
    ui_event_perror (err);
}

void
ui_event_thread_mutex_lock (pthread_mutex_t mutex)
{
  const int err = pthread_mutex_lock (&mutex);

  if (__builtin_expect (err != 0, 0))
    ui_event_perror (err);
}

void
ui_event_thread_mutex_unlock (pthread_mutex_t mutex)
{
  const int err = pthread_mutex_unlock (&mutex);

  if (__builtin_expect (err != 0, 0))
    ui_event_perror (err);
}
