#include <windows.h>

#include <commctrl.h>

#include <controlsigs.h>
#include <uipriv.h>

#include <ui/date_time_picker.h>
#include <ui/userbugs.h>

#include "datetimepicker.h"
#include "debug.h"
#include "init.h"
#include "utf16.h"
#include "winpublic.h"

#include <cstddef>
#include <time.h>

static WCHAR *
expandYear (const WCHAR *dts, const int n)
{
  // The real date/time picker does a manual replacement of "yy" with "yyyy" for DTS_SHORTDATECENTURYFORMAT.
  // Because we're also duplicating its functionality (see below), we have to do it too.

  int ny = 0;

  auto *const out
      = static_cast<WCHAR *> (uiprivAlloc (static_cast<unsigned long long> (n * 3) * sizeof (WCHAR), "WCHAR[]"));

  WCHAR *q = out;

  for (const WCHAR *p = dts; *p != L'\0'; p++)
    {
      // first, if the current character is a y, increment the number of consecutive ys
      // otherwise, stop counting, and if there were only two, add two more to make four
      if (*p != L'y')
        {
          if (ny == 2)
            {
              *q++ = L'y';
              *q++ = L'y';
            }
          ny = 0;
        }

      else
        {
          ny++;
        }

      // next, handle quoted blocks
      // we do this AFTER the above so yy'abc' becomes yyyy'abc' and not yy'abc'yy
      // this handles the case of 'a''b' elegantly as well
      if (*p == L'\'')
        {
          // copy the opening quote
          *q++ = *p;

          // copy the contents
          for (;;)
            {
              p++;
              if (*p == L'\'')
                break;

              if (*p == L'\0')
                uiprivImplBug ("unterminated quote in system-provided locale date string in expandYear()");

              *q++ = *p;
            }
          // and fall through to copy the closing quote
        }

      // copy the current character
      *q++ = *p;
    }

  // handle trailing yy
  if (ny == 2)
    {
      *q++ = L'y';
      *q++ = L'y';
    }

  *q++;

  *q = L'\0';

  return out;
}

static void
setDateTimeFormat (const HWND hwnd)
{
  // Windows has no combined date/time prebuilt constant; we have to build the format string ourselves

  const int ndate = GLI (LOCALE_SSHORTDATE, NULL, 0);

  if (ndate == 0)
    (void)logLastError (L"error getting date string length");

  auto *date = static_cast<WCHAR *> (uiprivAlloc (ndate * sizeof (WCHAR), "WCHAR[]"));
  if (GLI (LOCALE_SSHORTDATE, date, ndate) == 0)
    (void)logLastError (L"error geting date string");

  WCHAR *unexpandedDate = date;

  date = expandYear (unexpandedDate, ndate);
  uiprivFree (unexpandedDate);

  const int ntime = GLI (LOCALE_STIMEFORMAT, NULL, 0);
  if (ntime == 0)
    (void)logLastError (L"error getting time string length");

  auto *time = static_cast<WCHAR *> (uiprivAlloc (ntime * sizeof (WCHAR), "WCHAR[]"));
  if (GLI (LOCALE_STIMEFORMAT, time, ntime) == 0)
    (void)logLastError (L"error geting time string");

  WCHAR *datetime = strf (L"%s %s", date, time);
  if (SendMessageW (hwnd, DTM_SETFORMAT, 0, reinterpret_cast<LPARAM> (datetime)) == 0)
    (void)logLastError (L"error applying format string to date/time picker");

  uiprivFree (datetime);
  uiprivFree (time);
  uiprivFree (date);
}

static void
uiDateTimePickerDestroy (uiControl *c)
{
  auto *d = reinterpret_cast<uiDateTimePicker *> (c);

  uiWindowsUnregisterReceiveWM_WININICHANGE (d->hwnd);
  uiWindowsUnregisterWM_NOTIFYHandler (d->hwnd);
  uiWindowsEnsureDestroyWindow (d->hwnd);
  uiFreeControl (uiControl (d));
}

static uintptr_t
uiDateTimePickerHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiDateTimePicker *> (c)->hwnd);
}

static uiControl *
uiDateTimePickerParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiDateTimePickerSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiDateTimePickerToplevel (uiControl *)
{
  return 0;
}

static int
uiDateTimePickerVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiDateTimePickerShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiDateTimePicker *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiDateTimePickerHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiDateTimePicker *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiDateTimePickerEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiDateTimePickerEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiDateTimePickerDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiDateTimePickerSyncEnableState (uiWindowsControl *c, const int enabled)
{
  if (uiWindowsShouldStopSyncEnableState (c, enabled) != 0)
    return;

  EnableWindow (reinterpret_cast<uiDateTimePicker *> (c)->hwnd, enabled);
}

static void
uiDateTimePickerSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiDateTimePicker *> (c)->hwnd, parent);
}

static void
uiDateTimePickerMinimumSizeChanged (uiWindowsControl *c)
{
  if (uiWindowsControlTooSmall (c) != 0)
    uiWindowsControlContinueMinimumSizeChanged (c);
}

static void
uiDateTimePickerLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiDateTimePicker *> (c)->hwnd, r);
}

static void
uiDateTimePickerAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiDateTimePicker *> (c)->hwnd, controlID, insertAfter);
}

static void
uiDateTimePickerChildVisibilityChanged (uiWindowsControl *)
{
}

static void
uiDateTimePickerMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const uiDateTimePicker *d = uiDateTimePicker (c);
  SIZE                    s;
  uiWindowsSizing         sizing;
  int                     y;

  s.cx = 0;
  s.cy = 0;
  SendMessageW (d->hwnd, DTM_GETIDEALSIZE, 0, reinterpret_cast<LPARAM> (&s));
  *width = s.cx;

  y = entryHeight;
  uiWindowsGetSizing (d->hwnd, &sizing);
  uiWindowsSizingDlgUnitsToPixels (&sizing, nullptr, &y);
  *height = y;
}

static BOOL
// ReSharper disable once CppParameterMayBeConstPtrOrRef
onWM_NOTIFY (uiControl *c, HWND, NMHDR *nmhdr, LRESULT *lResult)
{
  auto *d = uiDateTimePicker (c);

  if (nmhdr->code != DTN_DATETIMECHANGE)
    return FALSE;

  (*d->onChanged) (d, d->onChangedData);

  *lResult = 0;

  return TRUE;
}

static void
fromSystemTime (const SYSTEMTIME *systime, tm *time)
{
  ZeroMemory (time, sizeof (tm));
  time->tm_sec   = systime->wSecond;
  time->tm_min   = systime->wMinute;
  time->tm_hour  = systime->wHour;
  time->tm_mday  = systime->wDay;
  time->tm_mon   = systime->wMonth - 1;
  time->tm_year  = systime->wYear - 1900;
  time->tm_wday  = systime->wDayOfWeek;
  time->tm_isdst = -1;
}

static void
toSystemTime (const tm *time, SYSTEMTIME *systime)
{
  ZeroMemory (systime, sizeof (SYSTEMTIME));
  systime->wYear      = time->tm_year + 1900;
  systime->wMonth     = time->tm_mon + 1;
  systime->wDayOfWeek = time->tm_wday;
  systime->wDay       = time->tm_mday;
  systime->wHour      = time->tm_hour;
  systime->wMinute    = time->tm_min;
  systime->wSecond    = time->tm_sec;
}

static void
defaultOnChanged (uiDateTimePicker *, void *)
{
  // do nothing
}

void
uiDateTimePickerTime (const uiDateTimePicker *d, tm *time)
{
  SYSTEMTIME systime;

  if (SendMessageW (d->hwnd, DTM_GETSYSTEMTIME, 0, reinterpret_cast<LPARAM> (&systime)) != GDT_VALID)
    (void)logLastError (L"error getting date and time");

  fromSystemTime (&systime, time);
}

void
uiDateTimePickerSetTime (const uiDateTimePicker *d, const tm *time)
{
  SYSTEMTIME systime;

  toSystemTime (time, &systime);
  if (SendMessageW (d->hwnd, DTM_SETSYSTEMTIME, GDT_VALID, reinterpret_cast<LPARAM> (&systime)) == 0)
    (void)logLastError (L"error setting date and time");
}

void
uiDateTimePickerOnChanged (uiDateTimePicker *d, void (*f) (uiDateTimePicker *, void *), void *data)
{
  d->onChanged     = f;
  d->onChangedData = data;
}

static uiDateTimePicker *
finishNewDateTimePicker (const DWORD style)
{
  auto *d = reinterpret_cast<uiDateTimePicker *> (
      uiWindowsAllocControl (sizeof (uiDateTimePicker), uiDateTimePickerSignature, "uiDateTimePicker"));

  auto *control      = reinterpret_cast<uiControl *> (d);
  control->Destroy   = uiDateTimePickerDestroy;
  control->Disable   = uiDateTimePickerDisable;
  control->Enable    = uiDateTimePickerEnable;
  control->Enabled   = uiDateTimePickerEnabled;
  control->Handle    = uiDateTimePickerHandle;
  control->Hide      = uiDateTimePickerHide;
  control->Parent    = uiDateTimePickerParent;
  control->SetParent = uiDateTimePickerSetParent;
  control->Show      = uiDateTimePickerShow;
  control->Toplevel  = uiDateTimePickerToplevel;
  control->Visible   = uiDateTimePickerVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (d);
  windows_control->AssignControlIDZOrder  = uiDateTimePickerAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiDateTimePickerChildVisibilityChanged;
  windows_control->LayoutRect             = uiDateTimePickerLayoutRect;
  windows_control->MinimumSize            = uiDateTimePickerMinimumSize;
  windows_control->MinimumSizeChanged     = uiDateTimePickerMinimumSizeChanged;
  windows_control->SetParentHWND          = uiDateTimePickerSetParentHWND;
  windows_control->SyncEnableState        = uiDateTimePickerSyncEnableState;
  windows_control->visible                = 1;
  windows_control->enabled                = 1;

  d->hwnd = uiWindowsEnsureCreateControlHWND (WS_EX_CLIENTEDGE, DATETIMEPICK_CLASSW, L"", style | WS_TABSTOP,
                                              hInstance, nullptr, TRUE);

  uiWindowsRegisterReceiveWM_WININICHANGE (d->hwnd);

  uiWindowsRegisterWM_NOTIFYHandler (d->hwnd, onWM_NOTIFY, uiControl (d));

  uiDateTimePickerOnChanged (d, defaultOnChanged, nullptr);

  return d;
}

static LRESULT CALLBACK
datetimepickerSubProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam,
                       const UINT_PTR uIdSubclass, DWORD_PTR)
{
  switch (uMsg)
    {
    case WM_WININICHANGE:
      {
        setDateTimeFormat (hwnd);
        return 0;
      }

    case WM_NCDESTROY:
      {
        if (RemoveWindowSubclass (hwnd, datetimepickerSubProc, uIdSubclass) == FALSE)
          (void)logLastError (L"error removing date-time picker locale change handling subclass");
        break;
      }

    default:;
    }

  return DefSubclassProc (hwnd, uMsg, wParam, lParam);
}

uiDateTimePicker *
uiNewDateTimePicker ()
{
  uiDateTimePicker *d = finishNewDateTimePicker (0);

  setDateTimeFormat (d->hwnd);

  if (SetWindowSubclass (d->hwnd, datetimepickerSubProc, 0, reinterpret_cast<DWORD_PTR> (d)) == FALSE)
    (void)logLastError (L"error subclassing date-time-picker to assist in locale change handling");

  return d;
}

uiDateTimePicker *
uiNewDatePicker ()
{
  return finishNewDateTimePicker (DTS_SHORTDATECENTURYFORMAT);
}

uiDateTimePicker *
uiNewTimePicker ()
{
  return finishNewDateTimePicker (DTS_TIMEFORMAT);
}
