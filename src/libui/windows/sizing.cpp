#include "sizing.h"

#include "debug.h"
#include "init.h"

void
getSizing (const HWND hwnd, uiWindowsSizing *sizing, const HFONT font)
{
  TEXTMETRICW tm;
  SIZE        size;

  const HDC dc = GetDC (hwnd);

  if (dc == NULL)
    (void)logLastError (L"error getting DC");

  auto *const prevfont = static_cast<HFONT> (SelectObject (dc, font));

  if (prevfont == NULL)
    (void)logLastError (L"error loading control font into device context");

  ZeroMemory (&tm, sizeof (TEXTMETRICW));

  if (GetTextMetricsW (dc, &tm) == 0)
    (void)logLastError (L"error getting text metrics");

  if (GetTextExtentPoint32W (dc, L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz", 52, &size) == 0)
    (void)logLastError (L"error getting text extent point");

  sizing->BaseX           = static_cast<int> ((size.cx / 26 + 1) / 2);
  sizing->BaseY           = static_cast<int> (tm.tmHeight);
  sizing->InternalLeading = tm.tmInternalLeading;

  if (SelectObject (dc, prevfont) != font)
    (void)logLastError (L"error restoring previous font into device context");

  if (ReleaseDC (hwnd, dc) == 0)
    (void)logLastError (L"error releasing DC");
}

void
uiWindowsGetSizing (const HWND hwnd, uiWindowsSizing *sizing)
{
  getSizing (hwnd, sizing, hMessageFont);
}

#define dlgUnitsToX(dlg, baseX) MulDiv ((dlg), (baseX), 4)
#define dlgUnitsToY(dlg, baseY) MulDiv ((dlg), (baseY), 8)

void
uiWindowsSizingDlgUnitsToPixels (const uiWindowsSizing *sizing, int *x, int *y)
{
  if (x != NULL)
    *x = dlgUnitsToX (*x, sizing->BaseX);

  if (y != NULL)
    *y = dlgUnitsToY (*y, sizing->BaseY);
}

#define winXPadding 4
#define winYPadding 4

void
uiWindowsSizingStandardPadding (const uiWindowsSizing *sizing, int *x, int *y)
{
  if (x != NULL)
    *x = dlgUnitsToX (winXPadding, sizing->BaseX);

  if (y != NULL)
    *y = dlgUnitsToY (winYPadding, sizing->BaseY);
}
