#include "events.h"

#include <map>
#include <ui/control.h>
#include <ui/userbugs.h>

struct handler
{
  BOOL (*commandHandler) (uiControl *, HWND, WORD, LRESULT *);

  BOOL (*notifyHandler) (uiControl *, HWND, NMHDR *, LRESULT *);

  BOOL (*hscrollHandler) (uiControl *, HWND, WORD, LRESULT *);

  uiControl *c;

  handler ()
  {
    this->commandHandler = nullptr;
    this->notifyHandler  = nullptr;
    this->hscrollHandler = nullptr;
    this->c              = nullptr;
  }
};

static std::map<HWND, handler> handlers;

void
uiWindowsRegisterWM_COMMANDHandler (const HWND hwnd, BOOL (*handler) (uiControl *, HWND, WORD, LRESULT *),
                                    uiControl *c)
{
  if (handlers[hwnd].commandHandler != NULL)
    uiprivImplBug ("already registered a WM_COMMAND handler to window handle %p", hwnd);

  handlers[hwnd].commandHandler = handler;

  handlers[hwnd].c = c;
}

void
uiWindowsRegisterWM_NOTIFYHandler (const HWND hwnd, BOOL (*handler) (uiControl *, HWND, NMHDR *, LRESULT *),
                                   uiControl *c)
{
  if (handlers[hwnd].notifyHandler != NULL)
    uiprivImplBug ("already registered a WM_NOTIFY handler to window handle %p", hwnd);

  handlers[hwnd].notifyHandler = handler;

  handlers[hwnd].c = c;
}

void
uiWindowsRegisterWM_HSCROLLHandler (const HWND hwnd, BOOL (*handler) (uiControl *, HWND, WORD, LRESULT *),
                                    uiControl *c)
{
  if (handlers[hwnd].hscrollHandler != NULL)
    uiprivImplBug ("already registered a WM_HSCROLL handler to window handle %p", hwnd);

  handlers[hwnd].hscrollHandler = handler;

  handlers[hwnd].c = c;
}

void
uiWindowsUnregisterWM_COMMANDHandler (const HWND hwnd)
{
  if (handlers[hwnd].commandHandler == NULL)
    uiprivImplBug ("window handle %p not registered to receive WM_COMMAND events", hwnd);

  handlers[hwnd].commandHandler = NULL;
}

void
uiWindowsUnregisterWM_NOTIFYHandler (const HWND hwnd)
{
  if (handlers[hwnd].notifyHandler == NULL)
    uiprivImplBug ("window handle %p not registered to receive WM_NOTIFY events", hwnd);

  handlers[hwnd].notifyHandler = NULL;
}

void
uiWindowsUnregisterWM_HSCROLLHandler (const HWND hwnd)
{
  if (handlers[hwnd].hscrollHandler == NULL)
    uiprivImplBug ("window handle %p not registered to receive WM_HSCROLL events", hwnd);

  handlers[hwnd].hscrollHandler = NULL;
}

template <typename T>
static BOOL
shouldRun (const HWND hwnd, T method)
{
  if (hwnd == NULL)
    return FALSE;

  // don't bounce back if to the utility window, in which case act as if the message was ignored
  if (IsChild (utilWindow, hwnd) != 0)
    return FALSE;

  return method != NULL;
}

BOOL
runWM_COMMAND (const WPARAM wParam, const LPARAM lParam, LRESULT *lResult)
{

  auto *hwnd = reinterpret_cast<HWND> (lParam); // NOLINT(*-no-int-to-ptr)

  const WORD arg3 = HIWORD (wParam);

  BOOL (*handler) (uiControl *, HWND, WORD, LRESULT *) = handlers[hwnd].commandHandler;

  uiControl *c = handlers[hwnd].c;

  if (shouldRun (hwnd, handler) != 0)
    return (*handler) (c, hwnd, arg3, lResult);

  return FALSE;
}

BOOL
runWM_NOTIFY (WPARAM, const LPARAM lParam, LRESULT *lResult)
{

  auto *const arg3 = reinterpret_cast<NMHDR *> (lParam); // NOLINT(*-no-int-to-ptr)

  const HWND hwnd = arg3->hwndFrom;

  BOOL (*handler) (uiControl *, HWND, NMHDR *, LRESULT *) = handlers[hwnd].notifyHandler;

  uiControl *c = handlers[hwnd].c;

  if (shouldRun (hwnd, handler) != 0)
    return (*handler) (c, hwnd, arg3, lResult);

  return FALSE;
}

BOOL
runWM_HSCROLL (const WPARAM wParam, const LPARAM lParam, LRESULT *lResult)
{

  auto *const hwnd = reinterpret_cast<HWND> (lParam); // NOLINT(*-no-int-to-ptr)

  const WORD arg3 = LOWORD (wParam);

  BOOL (*handler) (uiControl *, HWND, WORD, LRESULT *) = handlers[hwnd].hscrollHandler;

  uiControl *c = handlers[hwnd].c;

  if (shouldRun (hwnd, handler) != 0)
    return (*handler) (c, hwnd, arg3, lResult);

  return FALSE;
}

static std::map<HWND, bool> wininichanges;

void
uiWindowsRegisterReceiveWM_WININICHANGE (const HWND hwnd)
{
  if (wininichanges[hwnd])
    uiprivImplBug ("window handle %p already subscribed to receive WM_WINICHANGEs", hwnd);

  wininichanges[hwnd] = true;
}

void
uiWindowsUnregisterReceiveWM_WININICHANGE (const HWND hwnd)
{
  if (!wininichanges[hwnd])
    uiprivImplBug ("window handle %p not registered to receive WM_WININICHANGEs", hwnd);

  wininichanges[hwnd] = false;
}

void
issueWM_WININICHANGE (const WPARAM wParam, const LPARAM lParam)
{
  for (const auto &iter : wininichanges)
    SendMessageW (iter.first, WM_WININICHANGE, wParam, lParam);
}
