#pragma once

#include <stdlib.h>

#if defined(__cplusplus)
#define API extern "C"
#else
#define API
#endif

/**
 * @brief This constant is provided because M_PI is nonstandard.
 * @see http://oeis.org/A000796.
 */
#define uiPi 3.14159265358979323846264338327950288419716939937510582097494459

/**
 * @brief Represents the return value from one of libui's various ForEach functions.
 */
typedef enum uiForEach
{
  uiForEachContinue, //!<
  uiForEachStop,     //!<
} uiForEach;

/**
 * @brief LibUI initialization options.
 */
typedef struct uiInitOptions uiInitOptions;

struct uiInitOptions
{
  /**
   * @brief struct size
   */
  size_t Size;
};

/**
 * @brief Callback for functions queued to run on the UI thread
 * @param data user-defined data
 */
typedef void (uiQueueCallback) (void *data);

/**
 * @brief Initializes the LibUI library.
 * @param options @p uiInitOptions
 * @return @p NULL when successful, an error message on failure
 */
API const char *uiInit (uiInitOptions *options);

/**
 * @brief Frees all resources used by the LibUI library.
 */
API void uiUninit (void);

/**
 * @brief Frees errors returned by @p uiInit
 * @param err string
 */
API void uiFreeInitError (const char *err);

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
 * @brief Queues a function to run at a given interval on the UI thread
 * @param milliseconds interval
 * @param f pointer to the callback function
 * @param data to pass to the callback function
 */
API void uiTimer (int milliseconds, uiQueueCallback *f, void *data);

/**
 * @brief Registers a callback to invoke when the user-interface is about to shutdown
 * @param f pointer to the callback function
 * @param data to pass to the callback function
 */
API void uiOnShouldQuit (uiQueueCallback *f, void *data);

/**
 * @brief Free the memory of a returned string.
 *
 * Every time a string is returned from the library, this method should be called.
 *
 * Here are some examples of such functions:
 * @li @p uiWindowTitle
 * @li @p uiOpenFile
 * @li @p uiSaveFile
 * @li @p uiEntryText
 *
 * @param text The string to free memory
 * @remark This is not required for input strings. For example, with @p uiWindowSetTitle.
 */
API void uiFreeText (char *text);
