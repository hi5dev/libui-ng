#include "debug.h"

#include "utf16.h"

#include <stdio.h>
#include <uipriv.h>

static void
printDebug (const WCHAR *msg)
{
  OutputDebugStringW (msg);
}

HRESULT
_logLastError (debugargs, const WCHAR *s)
{
  const WCHAR *formatted;

  const DWORD le = GetLastError ();

  static constexpr auto flags
      = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

  const DWORD r = FormatMessageW (flags, nullptr, le, 0, reinterpret_cast<LPWSTR> (&formatted), 0, nullptr);

  if (r != 0)
    formatted = L"\n";

  WCHAR *msg = strf (L"[libui] %s:%s:%s() %s: GetLastError() == %I32u %s", file, line, func, s, le, formatted);

  if (r != 0)
    // ReSharper disable once CppCStyleCast
    LocalFree ((HLOCAL)formatted);

  printDebug (msg);
  uiprivFree (msg);
  DebugBreak ();

  SetLastError (le);

  // a function does not have to set a last error
  // if the last error we get is actually 0, then HRESULT_FROM_WIN32(0) will return S_OK (0 cast to an HRESULT, since 0
  // <= 0), which we don't want prevent this by returning E_FAIL
  if (le == 0)
    return E_FAIL;

  return HRESULT_FROM_WIN32 (le);
}

HRESULT
_logHRESULT (debugargs, const WCHAR *s, const HRESULT hr)
{
  static constexpr auto flags
      = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

  const LPWSTR formatted = nullptr;

  (void)FormatMessageW (flags, nullptr, hr, 0, formatted, 0, nullptr);

  WCHAR *msg = strf (L"[libui] %s:%s:%s() %s: HRESULT == 0x%08I32X %s", file, line, func, s, hr, formatted);

  printDebug (msg);

  uiprivFree (msg);

  DebugBreak ();

  return hr;
}

void
uiprivRealBug (const char *file, const char *line, const char *func, const char *prefix, const char *format,
               va_list ap)
{
  va_list ap2;

  va_copy (ap2, ap);

  size_t n = _vscprintf (format, ap2);

  va_end (ap2);

  n++;

  const auto msg = static_cast<char *> (uiprivAlloc (n * sizeof (char), "char[]"));

  vsprintf_s (msg, n, format, ap);

  WCHAR *final = strf (L"[libui] %hs:%hs:%hs() %hs%hs\n", file, line, func, prefix, msg);

  uiprivFree (msg);

  printDebug (final);

  uiprivFree (final);

  DebugBreak ();
}
