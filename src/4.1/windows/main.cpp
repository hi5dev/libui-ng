#include <windows.h>

#include "main.h"

#include "areaevents.h"
#include "debug.h"
#include "init.h"
#include "uipriv_windows.hpp"
#include "utilwin.h"
#include "winutil.h"

#include <map>
#include <uipriv.h>

static HHOOK filter;

static std::map<uiprivTimer *, bool> timers;

static LRESULT CALLBACK
filterProc (const int code, const WPARAM wParam, const LPARAM lParam)
{
  auto *msg = reinterpret_cast<MSG *> (lParam);

  if (code < 0)
    goto callNext;

  if (areaFilter (msg) != 0)
    goto discard;

  // otherwise keep going
  goto callNext;

discard:
  return 1;

callNext:
  return CallNextHookEx (filter, code, wParam, lParam);
}

int
registerMessageFilter ()
{
  filter = SetWindowsHookExW (WH_MSGFILTER, filterProc, hInstance, GetCurrentThreadId ());

  return filter != nullptr; // NOLINT(*-implicit-bool-conversion)
}

void
unregisterMessageFilter ()
{
  if (UnhookWindowsHookEx (filter) == 0)
    (void)logLastError (L"error unregistering libui message filter");
}

static void
processMessage (MSG *msg)
{
  HWND correctParent;

  if (msg->hwnd != nullptr)
    correctParent = parentToplevel (msg->hwnd);

  else
    correctParent = GetActiveWindow ();

  if (correctParent != nullptr)
    if (IsDialogMessage (correctParent, msg) != 0)
      return;

  TranslateMessage (msg);
  DispatchMessageW (msg);
}

static int
waitMessage (MSG *msg)
{
  const int res = GetMessageW (msg, nullptr, 0, 0);

  if (res < 0)
    {
      (void)logLastError (L"error calling GetMessage()");
      return 0;
    }

  return res != 0; // NOLINT(*-implicit-bool-conversion)
}

void
uiMainSteps ()
{
}

static int
peekMessage (MSG *msg)
{
  const BOOL res = PeekMessageW (msg, nullptr, 0, 0, PM_REMOVE);

  if (res == 0)
    return 2;

  if (msg->message != WM_QUIT)
    return 1;

  return 0;
}

int
uiMainStep (const int wait)
{
  MSG msg;

  if (wait != 0)
    {
      if (waitMessage (&msg) == 0)
        return 0;

      processMessage (&msg);

      return 1;
    }

  if (peekMessage (&msg) == 1)
    processMessage (&msg);

  else
    return 0;

  return 1;
}

void
uiMain ()
{
  while (uiMainStep (1) != 0)
    ;
}

void
uiQuit ()
{
  PostQuitMessage (0);
}

void
uiQueueMain (void (*f) (void *data), void *data)
{
  if (PostMessageW (utilWindow, msgQueued, reinterpret_cast<WPARAM> (f), reinterpret_cast<LPARAM> (data)) == 0)
    (void)logLastError (L"error queueing function to run on main thread");
}

void
uiTimer (const int milliseconds, int (*f) (void *data), void *data)
{

  auto *timer = uiprivNew (uiprivTimer);
  timer->f    = f;
  timer->data = data;

  if (SetTimer (utilWindow, reinterpret_cast<UINT_PTR> (timer), milliseconds, nullptr) == 0)
    (void)logLastError (L"error calling SetTimer() in uiTimer()");

  timers[timer] = true;
}

void
uiprivFreeTimer (uiprivTimer *t)
{
  timers.erase (t);
  uiprivFree (t);
}

void
uiprivUninitTimers ()
{
  for (auto t = timers.begin (); t != timers.end (); ++t)
    uiprivFree (t->first);

  timers.clear ();
}
