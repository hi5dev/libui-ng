#include "utf16.h"
#include "uipriv.h"

#include <cstdarg>
#include <utf.h>

#include <sstream>

WCHAR *
toUTF16 (const char *str)
{
  uint32_t rune;

  if (*str == '\0')
    return emptyUTF16 ();

  // ReSharper disable once CppDFAUnusedValue
  size_t n = uiprivUTF8UTF16Count (str, 0);

  auto *const wstr = static_cast<WCHAR *> (uiprivAlloc ((n + 1) * sizeof (WCHAR), "WCHAR[]"));

  WCHAR *wp = wstr;

  while (*str != 0)
    {
      str = uiprivUTF8DecodeRune (str, 0, &rune);
      n   = uiprivUTF16EncodeRune (rune, wp);
      wp += n;
    }

  return wstr;
}

char *
toUTF8 (const WCHAR *wstr)
{
  uint32_t rune;

  if (*wstr == L'\0')
    return emptyUTF8 ();

  // ReSharper disable once CppDFAUnusedValue
  size_t n = uiprivUTF16UTF8Count (wstr, 0);

  auto *const str = static_cast<char *> (uiprivAlloc ((n + 1) * sizeof (char), "char[]"));

  char *sp = str;

  while (*wstr != 0U)
    {
      wstr = uiprivUTF16DecodeRune (wstr, 0, &rune);
      n    = uiprivUTF8EncodeRune (rune, sp);
      sp += n;
    }

  return str;
}

WCHAR *
utf16dup (const WCHAR *orig)
{

  const size_t len = wcslen (orig);
  auto *const  out = static_cast<WCHAR *> (uiprivAlloc ((len + 1) * sizeof (WCHAR), "WCHAR[]"));
  wcscpy_s (out, len + 1, orig);
  return out;
}

WCHAR *
strf (const WCHAR *format, ...) // NOLINT(*-dcl50-cpp)
{
  va_list ap;

  va_start (ap, format);

  WCHAR *str = vstrf (format, ap);

  va_end (ap);

  return str;
}

WCHAR *
vstrf (const WCHAR *format, va_list ap)
{
  va_list ap2;

  if (*format == L'\0')
    return emptyUTF16 ();

  va_copy (ap2, ap);

  size_t n = _vscwprintf (format, ap2);

  va_end (ap2);

  n++;

  auto *const buf = static_cast<WCHAR *> (uiprivAlloc (n * sizeof (WCHAR), "WCHAR[]"));

  (void)vswprintf_s (buf, n, format, ap);

  return buf;
}

char *
LFtoCRLF (const char *lfonly)
{
  const size_t len = strlen (lfonly);

  auto *crlf = static_cast<char *> (uiprivAlloc ((len * 2 + 1) * sizeof (char), "char[]"));

  char *out = crlf;

  for (size_t i = 0; i < len; i++)
    {
      if (*lfonly == '\n')
        *crlf++ = '\r';

      *crlf++ = *lfonly++;
    }

  *crlf = '\0';

  return out;
}

void
CRLFtoLF (char *s)
{
  char *t = s;

  for (; *s != '\0'; s++)
    {
      // be sure to preserve \rs that are genuinely there
      if (*s == '\r' && *(s + 1) == '\n')
        continue;

      *t++ = *s;
    }

  *t = '\0';

  // pad out the rest of t, just to be safe
  while (t != s)
    *t++ = '\0';
}

WCHAR *
ftoutf16 (const double d)
{
  std::wostringstream ss;
  std::wstring        s;

  ss << d;
  s = ss.str ();
  return utf16dup (s.c_str ());
}

WCHAR *
itoutf16 (const int i)
{
  std::wostringstream ss;
  std::wstring        s;

  ss << i;
  s = ss.str ();
  return utf16dup (s.c_str ());
}
