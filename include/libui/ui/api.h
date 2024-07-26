#pragma once

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
  uiForEachContinue, //!< continue the loop
  uiForEachStop,     //!< break the loop
} uiForEach;

/**
 * @brief Callback for functions queued to run on the UI thread
 * @param data user-defined data
 */
typedef void (uiQueueCallback) (void *data);

/**
 * @brief Queues a function to run at a given interval on the UI thread
 * @param milliseconds interval
 * @param f pointer to the callback function
 * @param data to pass to the callback function
 */
API void uiTimer (int milliseconds, uiQueueCallback *f, void *data);

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
