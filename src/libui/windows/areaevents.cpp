#include "areaevents.h"
#include "area.h"
#include "areautil.h"
#include "debug.h"
#include "winutil.h"

#include <commctrl.h>
#include <windowsx.h>

static uiModifiers
getModifiers ()
{
  auto m = 0;

  if ((GetKeyState (VK_CONTROL) & 0x80) != 0)
    m |= uiModifierCtrl;

  if ((GetKeyState (VK_MENU) & 0x80) != 0)
    m |= uiModifierAlt;

  if ((GetKeyState (VK_SHIFT) & 0x80) != 0)
    m |= uiModifierShift;

  if ((GetKeyState (VK_LWIN) & 0x80) != 0)
    m |= uiModifierSuper;

  if ((GetKeyState (VK_RWIN) & 0x80) != 0)
    m |= uiModifierSuper;

  return static_cast<uiModifiers> (m);
}

/**
 * @brief Windows doesn't natively support mouse crossing events.
 *
 * @p TrackMouseEvent and its comctl32.dll wrapper @p _TrackMouseEvent both allow for a window to receive the
 * @p WM_MOUSELEAVE message when the mouse leaves the client area. There's no equivalent @p WM_MOUSEENTER because it
 * can be simulated.
 *
 * Unfortunately, @p WM_MOUSELEAVE does not get generated while the mouse is captured. We need to capture for drag
 * behavior to work properly, so this isn't going to mix well.
 *
 * So what we do:
 * - on @p WM_MOUSEMOVE, if we don't have the capture, start tracking
      - this will handle the case of the capture being released while still in the area
 * - on @p WM_MOUSELEAVE, mark that we are no longer tracking
 *    - Windows has already done the work of that for us; it's just a flag we use for the next part
 * - when starting capture, stop tracking if we are tracking
 * - if capturing, manually check if the pointer is in the client rect on each area event
 */
static void
track (uiArea *a, const BOOL tracking)
{
  TRACKMOUSEEVENT tm;

  if (a->tracking == tracking)
    return;

  a->tracking = tracking;

  ZeroMemory (&tm, sizeof (TRACKMOUSEEVENT));

  tm.cbSize  = sizeof (TRACKMOUSEEVENT);
  tm.dwFlags = TME_LEAVE;

  if (a->tracking == 0)
    tm.dwFlags |= TME_CANCEL;

  tm.hwndTrack = a->hwnd;

  if (_TrackMouseEvent (&tm) == 0)
    (void)logLastError (L"error setting up mouse tracking");
}

static void
capture (uiArea *a, const BOOL capturing)
{
  if (a->capturing == capturing)
    return;

  a->capturing = capturing;

  if (a->capturing != 0)
    {
      track (a, FALSE);
      SetCapture (a->hwnd);
    }

  else if (ReleaseCapture () == 0)
    {
      (void)logLastError (L"error releasing capture on drag");
    }
}

static void
areaMouseEvent (uiArea *a, const int down, const int up, const WPARAM wParam, const LPARAM lParam)
{
  uiAreaMouseEvent me;

  RECT client;

  double xpix;
  double ypix;

  if (a->capturing != 0)
    {
      POINT clientpt;
      clientpt.x = GET_X_LPARAM (lParam);
      clientpt.y = GET_Y_LPARAM (lParam);
      uiWindowsEnsureGetClientRect (a->hwnd, &client);
      const BOOL inClient = PtInRect (&client, clientpt);
      if (inClient != 0 && a->inside == 0)
        {
          a->inside = TRUE;
          (*a->ah->MouseCrossed) (a->ah, a, 0);
          uiprivClickCounterReset (&a->cc);
        }

      else if (inClient == 0 && a->inside != 0)
        {
          a->inside = FALSE;
          (*a->ah->MouseCrossed) (a->ah, a, 1);
          uiprivClickCounterReset (&a->cc);
        }
    }

  xpix = static_cast<double> (GET_X_LPARAM (lParam));
  ypix = static_cast<double> (GET_Y_LPARAM (lParam));

  pixelsToDIP (a, &xpix, &ypix);
  me.X = xpix;
  me.Y = ypix;

  if (a->scrolling != 0)
    {
      me.X += a->hscrollpos;
      me.Y += a->vscrollpos;
    }

  loadAreaSize (a, nullptr, &me.AreaWidth, &me.AreaHeight);

  me.Down  = down;
  me.Up    = up;
  me.Count = 0;

  if (me.Down != 0)
    me.Count = uiprivClickCounterClick (&a->cc, me.Down, me.X, me.Y, // NOLINT(*-narrowing-conversions)
                                        GetMessageTime (), GetDoubleClickTime (),
                                        GetSystemMetrics (SM_CXDOUBLECLK) / 2, GetSystemMetrics (SM_CYDOUBLECLK) / 2);

  me.Modifiers = getModifiers ();

  int button = me.Down;

  if (button == 0)
    button = me.Up;

  me.Held1To64 = 0;

  if (button != 1 && (wParam & MK_LBUTTON) != 0)
    me.Held1To64 |= 1 << 0;

  if (button != 2 && (wParam & MK_MBUTTON) != 0)
    me.Held1To64 |= 1 << 1;

  if (button != 3 && (wParam & MK_RBUTTON) != 0)
    me.Held1To64 |= 1 << 2;

  if (button != 4 && (wParam & MK_XBUTTON1) != 0)
    me.Held1To64 |= 1 << 3;

  if (button != 5 && (wParam & MK_XBUTTON2) != 0)
    me.Held1To64 |= 1 << 4;

  if (me.Down != 0)
    capture (a, TRUE);

  if (me.Up != 0 && me.Held1To64 == 0)
    capture (a, FALSE);

  (*a->ah->MouseEvent) (a->ah, a, &me);
}

static void
onMouseEntered (uiArea *a)
{
  if (a->inside != 0)
    return;

  if (a->capturing != 0)
    return;

  track (a, TRUE);
  (*a->ah->MouseCrossed) (a->ah, a, 0);

  uiprivClickCounterReset (&a->cc);
}

static void
onMouseLeft (uiArea *a)
{
  a->tracking = FALSE;
  a->inside   = FALSE;

  (*a->ah->MouseCrossed) (a->ah, a, 1);

  uiprivClickCounterReset (&a->cc);
}

struct extkeymap
{
  WPARAM   vk;
  uiExtKey extkey;
};

static const extkeymap numpadExtKeys[] = {
  { VK_HOME,     uiExtKeyN7   },
  { VK_UP,       uiExtKeyN8   },
  { VK_PRIOR,    uiExtKeyN9   },
  { VK_LEFT,     uiExtKeyN4   },
  { VK_CLEAR,    uiExtKeyN5   },
  { VK_RIGHT,    uiExtKeyN6   },
  { VK_END,      uiExtKeyN1   },
  { VK_DOWN,     uiExtKeyN2   },
  { VK_NEXT,     uiExtKeyN3   },
  { VK_INSERT,   uiExtKeyN0   },
  { VK_DELETE,   uiExtKeyNDot },
  { VK_SNAPSHOT, uiExtKeyNone },
};

static const extkeymap extKeys[] = {
  { VK_ESCAPE,   uiExtKeyEscape    },
  { VK_INSERT,   uiExtKeyInsert    },
  { VK_DELETE,   uiExtKeyDelete    },
  { VK_HOME,     uiExtKeyHome      },
  { VK_END,      uiExtKeyEnd       },
  { VK_PRIOR,    uiExtKeyPageUp    },
  { VK_NEXT,     uiExtKeyPageDown  },
  { VK_UP,       uiExtKeyUp        },
  { VK_DOWN,     uiExtKeyDown      },
  { VK_LEFT,     uiExtKeyLeft      },
  { VK_RIGHT,    uiExtKeyRight     },
  { VK_F1,       uiExtKeyF1        },
  { VK_F2,       uiExtKeyF2        },
  { VK_F3,       uiExtKeyF3        },
  { VK_F4,       uiExtKeyF4        },
  { VK_F5,       uiExtKeyF5        },
  { VK_F6,       uiExtKeyF6        },
  { VK_F7,       uiExtKeyF7        },
  { VK_F8,       uiExtKeyF8        },
  { VK_F9,       uiExtKeyF9        },
  { VK_F10,      uiExtKeyF10       },
  { VK_F11,      uiExtKeyF11       },
  { VK_F12,      uiExtKeyF12       },
  { VK_ADD,      uiExtKeyNAdd      },
  { VK_SUBTRACT, uiExtKeyNSubtract },
  { VK_MULTIPLY, uiExtKeyNMultiply },
  { VK_DIVIDE,   uiExtKeyNDivide   },
  { VK_SNAPSHOT, uiExtKeyNone      },
};

static const struct
{
  WPARAM      vk;
  uiModifiers mod;
} modKeys[] = {
  { VK_CONTROL,  uiModifierCtrl  },
  { VK_LCONTROL, uiModifierCtrl  },
  { VK_RCONTROL, uiModifierCtrl  },
  { VK_MENU,     uiModifierAlt   },
  { VK_LMENU,    uiModifierAlt   },
  { VK_RMENU,    uiModifierAlt   },
  { VK_SHIFT,    uiModifierShift },
  { VK_LSHIFT,   uiModifierShift },
  { VK_RSHIFT,   uiModifierShift },
  { VK_LWIN,     uiModifierSuper },
  { VK_RWIN,     uiModifierSuper },
  { VK_SNAPSHOT, uiModifierNone  },
};

static int
areaKeyEvent (uiArea *a, const int up, const WPARAM wParam, const LPARAM lParam)
{
  uiAreaKeyEvent ke;
  int            i;

  ke.Key      = 0;
  ke.ExtKey   = uiExtKeyNone;
  ke.Modifier = uiModifierNone;

  ke.Modifiers = getModifiers ();

  ke.Up = up;

  // the numeric keypad keys when Num Lock is off are considered left-hand keys as the separate navigation buttons were
  // added later the numeric keypad Enter, however, is a right-hand key because it has the same virtual-key code as the
  // typewriter Enter
  if ((lParam & static_cast<LPARAM> (0x01000000 != 0)) != 0)
    {
      if (wParam == VK_RETURN)
        {
          ke.ExtKey = uiExtKeyNEnter;
          goto keyFound;
        }
    }

  else
    // this is special handling for numpad keys to ignore the state of Num Lock and Shift; see
    for (i = 0; numpadExtKeys[i].vk != VK_SNAPSHOT; i++)
      if (numpadExtKeys[i].vk == wParam)
        {
          ke.ExtKey = numpadExtKeys[i].extkey;
          goto keyFound;
        }

  for (i = 0; extKeys[i].vk != VK_SNAPSHOT; i++)
    if (extKeys[i].vk == wParam)
      {
        ke.ExtKey = extKeys[i].extkey;
        goto keyFound;
      }

  for (i = 0; modKeys[i].vk != VK_SNAPSHOT; i++)
    if (modKeys[i].vk == wParam)
      {
        ke.Modifier  = modKeys[i].mod;
        ke.Modifiers = static_cast<uiModifiers> (ke.Modifiers & ~ke.Modifier);
        goto keyFound;
      }

  // and finally everything else
  if (uiprivFromScancode (lParam >> 16 & 0xFF, &ke) != 0)
    goto keyFound;

  // not a supported key, assume unhandled
  // TODO the original code only did this if ke.Modifiers == 0 - why?
  return 0;

keyFound:
  return (*a->ah->KeyEvent) (a->ah, a, &ke);
}

// We don't handle the standard Windows keyboard messages directly, to avoid both the dialog manager and
// TranslateMessage(). Instead, we set up a message filter and do things there. That stuff is later in this file.
enum : uint16_t
{
  // start at 0x40 to avoid clobbering dialog messages
  msgAreaKeyDown = WM_USER + 0x40,
  msgAreaKeyUp,
};

BOOL
areaDoEvents (uiArea *a, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, LRESULT *lResult)
{
  switch (uMsg)
    {
    case WM_ACTIVATE:
      {
        uiprivClickCounterReset (&a->cc);
        *lResult = 0;
        return TRUE;
      }

    case WM_MOUSEMOVE:
      {
        onMouseEntered (a);
        areaMouseEvent (a, 0, 0, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_MOUSELEAVE:
      {
        onMouseLeft (a);
        *lResult = 0;
        return TRUE;
      }

    case WM_LBUTTONDOWN:
      {
        SetFocus (a->hwnd);
        areaMouseEvent (a, 1, 0, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_LBUTTONUP:
      {
        areaMouseEvent (a, 0, 1, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_MBUTTONDOWN:
      {
        SetFocus (a->hwnd);
        areaMouseEvent (a, 2, 0, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_MBUTTONUP:
      {
        areaMouseEvent (a, 0, 2, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_RBUTTONDOWN:
      {
        SetFocus (a->hwnd);
        areaMouseEvent (a, 3, 0, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_RBUTTONUP:
      {
        areaMouseEvent (a, 0, 3, wParam, lParam);
        *lResult = 0;
        return TRUE;
      }

    case WM_XBUTTONDOWN:
      {
        SetFocus (a->hwnd);
        areaMouseEvent (a, GET_XBUTTON_WPARAM (wParam) + 3, 0, GET_KEYSTATE_WPARAM (wParam), lParam);
        *lResult = TRUE;
        return TRUE;
      }

    case WM_XBUTTONUP:
      {
        areaMouseEvent (a, 0, GET_XBUTTON_WPARAM (wParam) + 3, GET_KEYSTATE_WPARAM (wParam), lParam);
        *lResult = TRUE;
        return TRUE;
      }

    case WM_CAPTURECHANGED:
      {
        if (a->capturing != 0)
          {
            a->capturing = FALSE;
            (*a->ah->DragBroken) (a->ah, a);
          }
        *lResult = 0;
        return TRUE;
      }

    case msgAreaKeyDown:
      {
        *lResult = static_cast<LRESULT> (areaKeyEvent (a, 0, wParam, lParam));
        return TRUE;
      }

    case msgAreaKeyUp:
      {
        *lResult = static_cast<LRESULT> (areaKeyEvent (a, 1, wParam, lParam));
        return TRUE;
      }

    default:
      break;
    }
  return FALSE;
}

BOOL
areaFilter (const MSG *msg)
{
  if (msg->hwnd == nullptr)
    return FALSE;

  if (windowClassOf (msg->hwnd, areaClass, nullptr) != 0)
    return FALSE;

  LRESULT handled = 0;
  switch (msg->message)
    {
    case WM_KEYDOWN:
      [[fallthrough]];

    case WM_SYSKEYDOWN:
      handled = SendMessageW (msg->hwnd, msgAreaKeyDown, msg->wParam, msg->lParam);
      break;

    case WM_KEYUP:
      [[fallthrough]];

    case WM_SYSKEYUP:
      handled = SendMessageW (msg->hwnd, msgAreaKeyUp, msg->wParam, msg->lParam);
      break;

    default:
      break;
    }
  return static_cast<BOOL> (handled);
}
