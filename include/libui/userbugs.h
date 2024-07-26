#pragma once

#include "api.h"

#include <stdarg.h>

#define uiprivMacro_ns2(s) #s
#define uiprivMacro_ns(s)  uiprivMacro_ns2 (s)
#define uiprivImplBug(...) uiprivDoImplBug (__FILE__, uiprivMacro_ns (__LINE__), __FUNCTION__, __VA_ARGS__)
#define uiprivUserBug(...) uiprivDoUserBug (__FILE__, uiprivMacro_ns (__LINE__), __FUNCTION__, __VA_ARGS__)

/**
 * @brief
 * @param file
 * @param line
 * @param func
 * @param prefix
 * @param format
 * @param ap
 */
API void uiprivRealBug (const char *file, const char *line, const char *func, const char *prefix, const char *format,
                        va_list ap);

/**
 * @brief
 * @param file
 * @param line
 * @param func
 * @param format
 * @param ...
 */
API void uiprivDoImplBug (const char *file, const char *line, const char *func, const char *format, ...);

/**
 * @brief
 * @param file
 * @param line
 * @param func
 * @param format
 * @param ...
 */
API void uiprivDoUserBug (const char *file, const char *line, const char *func, const char *format, ...);

API void uiUserBugCannotSetParentOnToplevel (const char *type);
