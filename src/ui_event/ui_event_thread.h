#pragma once

#include <pthread.h>

/**
 * @brief @p pthread_mutex_t constructor.
 * @remark Errors will be printed to stderr.
 */
void ui_event_thread_mutex_init (     //!< @params
    pthread_mutex_t            mutex, //!< pthread mutex
    const pthread_mutexattr_t *attr   //!< pthread mutex attributes
);

/**
 * @brief @p pthread_mutex_t destructor.
 * @remark Errors will be printed to stderr.
 */
void ui_event_thread_mutex_destroy ( //!< @params
    pthread_mutex_t mutex            //!< pthread mutex
);

/**
 * @brief Locks a mutex.
 * @remark Errors will be printed to stderr.
 */
void ui_event_thread_mutex_lock ( //!< @params
    pthread_mutex_t mutex         //!< pthread mutex
);

/**
 * @brief Unlocks a mutex.
 * @remark Errors will be printed to stderr.
 */
void ui_event_thread_mutex_unlock ( //!< @params
    pthread_mutex_t mutex           //!< pthread mutex
);
