#pragma once

#include "api.h"

/**
 * @brief LibUI's main entry-point.
 * @remark this is a thread-blocking call
 */
API void uiMain (void);

/**
 * @brief LibUI's main loop.
 * @remark this is a thread-blocking call
 * @see uiMain
 */
API void uiMainSteps (void);

/**
 * @brief LibUI's message-dispatch function.
 * @param wait
 * @see uiMainSteps
 * @see uiMain
 */
API int uiMainStep (int wait);

/**
 * @brief Instructs LibUI to exit the main loop.
 */
API void uiQuit (void);

/**
 * @brief Queues a function to run on the UI thread
 * @param f pointer to the callback function
 * @param data to pass to the callback function
 */
API void uiQueueMain (uiQueueCallback *f, void *data);

/**
 * @brief Registers a callback to invoke when the user-interface is about to shutdown
 * @param f pointer to the callback function
 * @param data to pass to the callback function
 * @return non-zero to cancel
 */
API int uiOnShouldQuit (uiQueueCancelableCallback *f, void *data);
