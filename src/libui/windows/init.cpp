#include "init.h"
#include "alloc.h"
#include "area.h"
#include "container.h"
#include "d2dscratch.h"
#include "debug.h"
#include "draw.h"
#include "fontcollection.h"
#include "image.h"
#include "main.h"
#include "menu.h"
#include "utf16.h"
#include "utilwin.h"
#include "window.h"

#include <ui/init.h>
#include <uipriv.h>

HINSTANCE hInstance;
int       nCmdShow;

HFONT  hMessageFont;
HBRUSH hollowBrush;

static const char *
initerr (const char *message, const WCHAR *, const DWORD value)
{
  const LPWSTR sysmsg = nullptr;

  static constexpr auto flags
      = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

  (void)FormatMessageW (flags, nullptr, value, 0, sysmsg, 0, nullptr);

  WCHAR *wmessage = toUTF16 (message + 1);

  WCHAR *wout = strf (L"-error initializing libui: %s; code %I32d (0x%08I32X) %s", wmessage, value, value, sysmsg);

  uiprivFree (wmessage);

  const char *out = toUTF8 (wout);

  uiprivFree (wout);

  return out + 1;
}

// LONGTERM put this declaration in a common file
uiInitOptions uiprivOptions;

#define wantedICCClasses                                                                                              \
  (ICC_STANDARD_CLASSES | /* user32.dll controls */                                                                   \
   ICC_PROGRESS_CLASS |   /* progress bars */                                                                         \
   ICC_TAB_CLASSES |      /* tabs */                                                                                  \
   ICC_LISTVIEW_CLASSES | /* table headers */                                                                         \
   ICC_UPDOWN_CLASS |     /* spinboxes */                                                                             \
   ICC_BAR_CLASSES |      /* trackbar */                                                                              \
   ICC_DATE_CLASSES |     /* date/time picker */                                                                      \
   0)

const char *
uiInit (uiInitOptions *options)
{
  STARTUPINFOW         si;
  const char          *ce;
  HICON                hDefaultIcon;
  HCURSOR              hDefaultCursor;
  NONCLIENTMETRICSW    ncm;
  INITCOMMONCONTROLSEX icc;
  HRESULT              hr;

  uiprivOptions = *options;

  initAlloc ();

  nCmdShow = SW_SHOWDEFAULT;
  GetStartupInfoW (&si);
  if ((si.dwFlags & STARTF_USESHOWWINDOW) != 0)
    nCmdShow = si.wShowWindow;

  hDefaultIcon = LoadIconW (nullptr, IDI_APPLICATION);
  if (hDefaultIcon == nullptr)
    return ieLastErr ("loading default icon for window classes");

  hDefaultCursor = LoadCursorW (nullptr, IDC_ARROW);
  if (hDefaultCursor == nullptr)
    return ieLastErr ("loading default cursor for window classes");

  ce = initUtilWindow (hDefaultIcon, hDefaultCursor);
  if (ce != nullptr)
    return initerr (ce, L"GetLastError() ==", GetLastError ());

  if (registerWindowClass (hDefaultIcon, hDefaultCursor) == 0)
    return ieLastErr ("registering uiWindow window class");

  ZeroMemory (&ncm, sizeof (NONCLIENTMETRICSW));
  ncm.cbSize = sizeof (NONCLIENTMETRICSW);
  if (SystemParametersInfoW (SPI_GETNONCLIENTMETRICS, sizeof (NONCLIENTMETRICSW), &ncm, sizeof (NONCLIENTMETRICSW))
      == 0)
    return ieLastErr ("getting default fonts");
  hMessageFont = CreateFontIndirectW (&(ncm.lfMessageFont));
  if (hMessageFont == nullptr)
    return ieLastErr ("loading default messagebox font; this is the default UI font");

  if (initContainer (hDefaultIcon, hDefaultCursor) == 0)
    return ieLastErr ("initializing uiWindowsMakeContainer() window class");

  hollowBrush = static_cast<HBRUSH> (GetStockObject (HOLLOW_BRUSH));
  if (hollowBrush == nullptr)
    return ieLastErr ("getting hollow brush");

  ZeroMemory (&icc, sizeof (INITCOMMONCONTROLSEX));
  icc.dwSize = sizeof (INITCOMMONCONTROLSEX);
  icc.dwICC  = wantedICCClasses;
  if (InitCommonControlsEx (&icc) == 0)
    return ieLastErr ("initializing Common Controls");

  hr = CoInitialize (nullptr);
  if (hr != S_OK && hr != S_FALSE)
    return ieHRESULT ("initializing COM", hr);

  hr = initDraw ();
  if (hr != S_OK)
    return ieHRESULT ("initializing Direct2D", hr);

  hr = uiprivInitDrawText ();
  if (hr != S_OK)
    return ieHRESULT ("initializing DirectWrite", hr);

  if (registerAreaClass (hDefaultIcon, hDefaultCursor) == 0)
    return ieLastErr ("registering uiArea window class");

  if (registerMessageFilter () == 0)
    return ieLastErr ("registering libui message filter");

  if (registerD2DScratchClass (hDefaultIcon, hDefaultCursor) == 0)
    return ieLastErr ("initializing D2D scratch window class");

  hr = uiprivInitImage ();
  if (hr != S_OK)
    return ieHRESULT ("initializing WIC", hr);

  return nullptr;
}

void
uiUninit ()
{
  uiprivUninitTimers ();

  uiprivUninitImage ();

  uninitMenus ();

  unregisterD2DScratchClass ();

  unregisterMessageFilter ();

  unregisterArea ();

  uiprivUninitDrawText ();

  uninitDraw ();

  CoUninitialize ();

  if (DeleteObject (hollowBrush) == 0)
    (void)logLastError (L"error freeing hollow brush");

  uninitContainer ();

  if (DeleteObject (hMessageFont) == 0)
    (void)logLastError (L"error deleting control font");

  unregisterWindowClass ();

  uninitUtilWindow ();

  uninitAlloc ();
}

void
uiFreeInitError (const char *err)
{
  if (*(err - 1) == '-')
    uiprivFree (reinterpret_cast<void *> (*(err - 1)));
}

BOOL WINAPI
DllMain (const HINSTANCE hinstDLL, const DWORD fdwReason, LPVOID)
{
  if (fdwReason == DLL_PROCESS_ATTACH)
    hInstance = hinstDLL;

  return TRUE;
}
