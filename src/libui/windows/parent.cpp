#include "parent.h"
#include "events.h"

BOOL
handleParentMessages (HWND, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, LRESULT *lResult)
{
  switch (uMsg)
    {
    case WM_COMMAND:
      return runWM_COMMAND (wParam, lParam, lResult);

    case WM_NOTIFY:
      return runWM_NOTIFY (wParam, lParam, lResult);

    case WM_HSCROLL:
      return runWM_HSCROLL (wParam, lParam, lResult);

    default:
      return FALSE;
    }
}
