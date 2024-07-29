#include "text.h"
#include "debug.h"
#include "init.h"
#include "utf16.h"

#include <uipriv.h>

WCHAR *
windowTextAndLen (const HWND hwnd, LRESULT *len)
{
  const LRESULT n = SendMessageW (hwnd, WM_GETTEXTLENGTH, 0, 0);

  if (len != nullptr)
    *len = n;

  auto *const text = static_cast<WCHAR *> (uiprivAlloc ((n + 1) * sizeof (WCHAR), "WCHAR[]"));

  if (GetWindowTextW (hwnd, text, n + 1) != n) // NOLINT(*-narrowing-conversions)
    {
      (void)logLastError (L"error getting window text");

      *text = L'\0';

      if (len != nullptr)
        *len = 0;
    }

  return text;
}

WCHAR *
windowText (const HWND hwnd)
{
  return windowTextAndLen (hwnd, nullptr);
}

void
setWindowText (const HWND hwnd, const WCHAR *wtext)
{
  if (SetWindowTextW (hwnd, wtext) == 0)
    (void)logLastError (L"error setting window text");
}

void
uiFreeText (char *text)
{
  uiprivFree (text);
}

int
uiWindowsWindowTextHeight (const HWND hwnd)
{
  LRESULT len;
  int     lineCount = 1;

  WCHAR *text = windowTextAndLen (hwnd, &len);

  for (const WCHAR *start = text; start != text + len; start++)
    if (*start == L'\n')
      lineCount++;

  uiprivFree (text);

  return lineCount * labelHeight;
}

int
uiWindowsWindowTextWidth (const HWND hwnd)
{
  LRESULT len;
  WCHAR  *end;
  SIZE    size;

  int maxWidth = 0;

  WCHAR *text = windowTextAndLen (hwnd, &len);
  if (len == 0)
    goto noTextOrError;

  const HDC dc = GetDC (hwnd);
  if (dc == nullptr)
    {
      (void)logLastError (L"error getting DC");
      goto noTextOrError;
    }

  auto *const prevfont = static_cast<HFONT> (SelectObject (dc, hMessageFont));

  if (prevfont == nullptr)
    {
      (void)logLastError (L"error loading control font into device context");
      ReleaseDC (hwnd, dc);
      goto noTextOrError;
    }

  // calculate width of each line
  WCHAR *start = end = text;
  while (start != text + len)
    {
      while (*start == L'\n' && start != text + len)
        start++;

      if (start == text + len)
        break;

      end = start + 1;

      while (*end != L'\n' && end != text + len)
        end++;

      if (GetTextExtentPoint32W (dc, start, static_cast<int> (end - start), &size) == 0)
        (void)logLastError (L"error getting text extent point");

      else if (size.cx > maxWidth)
        maxWidth = size.cx;

      start = end;
    }

  if (SelectObject (dc, prevfont) != hMessageFont)
    (void)logLastError (L"error restoring previous font into device context");

  if (ReleaseDC (hwnd, dc) == 0)
    (void)logLastError (L"error releasing DC");

  uiprivFree (text);
  return maxWidth;

noTextOrError:
  uiprivFree (text);
  return 0;
}

char *
uiWindowsWindowText (const HWND hwnd)
{

  WCHAR *wtext = windowText (hwnd);
  char  *text  = toUTF8 (wtext);
  uiprivFree (wtext);
  return text;
}

void
uiWindowsSetWindowText (const HWND hwnd, const char *text)
{

  WCHAR *wtext = toUTF16 (text);
  setWindowText (hwnd, wtext);
  uiprivFree (wtext);
}

int
uiprivStricmp (const char *a, const char *b)
{

  WCHAR    *wa  = toUTF16 (a);
  WCHAR    *wb  = toUTF16 (b);
  const int ret = _wcsicmp (wa, wb);
  uiprivFree (wb);
  uiprivFree (wa);
  return ret;
}
