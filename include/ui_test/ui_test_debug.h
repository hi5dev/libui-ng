#pragma once

#include <stdio.h>

#if !defined(UI_TEST_DEBUG_FILE_ERR)
#define UI_TEST_DEBUG_FILE_ERR stderr
#endif

//! @param context to include in the message.
//! @see @ref _ui_test_debug_errno
#define ui_test_debug_errno(context) _ui_test_debug_errno (stderr, context, __FILE__, __LINE__)

//! @brief Prints the reason for the last error for methods that set @p errno to the given @p FILE stream.
void _ui_test_debug_errno ( //!< @params
    FILE       *stream,     //!< Where to write the error message.
    const char *context,    //!< Context to include in the message written to stderr.
    const char *file,       //!< Full path to the source file with the error.
    int         line        //!< Line number with the error.
);
