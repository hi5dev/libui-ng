#pragma once

#include <windows.h>

#define _ws2(m)  L##m
#define _ws(m)   _ws2 (m)
#define _ws2n(m) L## #m
#define _wsn(m)  _ws2n (m)

#define debugargs const WCHAR *file, const WCHAR *line, const WCHAR *func

#ifdef _MSC_VER
#define logLastError(s) _logLastError (_ws (__FILE__), _wsn (__LINE__), _ws (__FUNCTION__), s)
#else
#define logLastError(s)                                                                                               \
  _logLastError (_ws (__FILE__), _wsn (__LINE__), L"TODO none of the function name macros are macros in MinGW", s)
#endif

#ifdef _MSC_VER
#define logHRESULT(s, hr) _logHRESULT (_ws (__FILE__), _wsn (__LINE__), _ws (__FUNCTION__), s, hr)
#else
#define logHRESULT(s, hr)                                                                                             \
  _logHRESULT (_ws (__FILE__), _wsn (__LINE__), L"TODO none of the function name macros are macros in MinGW", s, hr)
#endif

extern HRESULT _logLastError (debugargs, const WCHAR *s);
extern HRESULT _logHRESULT (debugargs, const WCHAR *s, HRESULT hr);
