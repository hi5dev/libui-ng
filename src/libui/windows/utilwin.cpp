#include "utilwin.h"

#include "debug.h"
#include "events.h"
#include "init.h"
#include "main.h"
#include "parent.h"
#include "uipriv_windows.hpp"

#include <ui/main.h>
#include <uipriv.h>

/**
 * @brief The utility window is a special window that performs certain tasks internal to libui.
 *
 * It is not a message-only window, and it is always hidden and disabled.
 *
 * Its roles:
 * - It is the initial parent of all controls.
 * - When a control loses its parent, it also becomes that control's parent.
 * - It handles @p WM_QUERYENDSESSION requests.
 * - It handles @p WM_WININICHANGE and forwards the message to any child windows that request it.
 * - It handles executing functions queued to run by @p uiQueueMain
 */

#define utilWindowClass L"libui_utilWindowClass"

HWND utilWindow;

static LRESULT CALLBACK
utilWindowWndProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
  LRESULT lResult;

  if (handleParentMessages (hwnd, uMsg, wParam, lParam, &lResult) != FALSE)
    return lResult;

  switch (uMsg)
    {
    case WM_QUERYENDSESSION:
      {
        if (uiprivShouldQuit () != 0)
          {
            uiQuit ();
            return TRUE;
          }
        return FALSE;
      }

    case WM_WININICHANGE:
      {
        issueWM_WININICHANGE (wParam, lParam);
        return 0;
      }

    case msgQueued:
      {
        const auto qf = reinterpret_cast<void (*) (void *)> (wParam); // NOLINT(*-no-int-to-ptr)

        (*qf) (reinterpret_cast<void *> (lParam)); // NOLINT(*-no-int-to-ptr)

        return 0;
      }

    case WM_TIMER:
      {
        auto *timer = reinterpret_cast<uiprivTimer *> (wParam); // NOLINT(*-no-int-to-ptr)
        if ((*timer->f) (timer->data) == 0)
          {
            if (KillTimer (utilWindow, reinterpret_cast<UINT_PTR> (timer)) == 0)
              (void)logLastError (L"error calling KillTimer() to end uiTimer() procedure");
            uiprivFreeTimer (timer);
          }
        return 0;
      }

    default:
      return DefWindowProcW (hwnd, uMsg, wParam, lParam);
    }
}

const char *
initUtilWindow (const HICON hDefaultIcon, const HCURSOR hDefaultCursor)
{
  WNDCLASSW wc;
  ZeroMemory (&wc, sizeof (WNDCLASSW));

  wc.lpszClassName = utilWindowClass;
  wc.lpfnWndProc   = utilWindowWndProc;
  wc.hInstance     = hInstance;
  wc.hIcon         = hDefaultIcon;
  wc.hCursor       = hDefaultCursor;
  wc.hbrBackground = reinterpret_cast<HBRUSH> ((COLOR_BTNFACE + 1)); // NOLINT(*-no-int-to-ptr)

  if (RegisterClass (&wc) == 0)
    return "=registering utility window class";

  static constexpr auto width  = 100;
  static constexpr auto height = 100;
  utilWindow = CreateWindowExW (0, utilWindowClass, L"libui utility window", WS_OVERLAPPEDWINDOW, 0, 0, width, height,
                                nullptr, nullptr, hInstance, nullptr);

  if (utilWindow == nullptr)
    return "=creating utility window";

  EnableWindow (utilWindow, FALSE);

  return nullptr;
}

void
uninitUtilWindow ()
{
  if (DestroyWindow (utilWindow) == 0)
    (void)logLastError (L"error destroying utility window");

  if (UnregisterClass (utilWindowClass, hInstance) == 0)
    (void)logLastError (L"error unregistering utility window class");
}
