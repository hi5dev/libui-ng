#pragma once

#include <windows.h>

#include <ui/control.h>

#define uiWindowsControl(this) ((uiWindowsControl *)(this))

#define uiWindowsControlDefaultDestroy(type)                                                                          \
  static void type##Destroy (uiControl *c)                                                                            \
  {                                                                                                                   \
    uiWindowsEnsureDestroyWindow (type (c)->hwnd);                                                                    \
    uiFreeControl (c);                                                                                                \
  }

#define uiWindowsControlDefaultHandle(type)                                                                           \
  static uintptr_t type##Handle (uiControl *c) { return (uintptr_t)(type (c)->hwnd); }

#define uiWindowsControlDefaultParent(type)                                                                           \
  static uiControl *type##Parent (uiControl *c) { return uiWindowsControl (c)->parent; }

#define uiWindowsControlDefaultSetParent(type)                                                                        \
  static void type##SetParent (uiControl *c, uiControl *parent)                                                       \
  {                                                                                                                   \
    uiControlVerifySetParent (c, parent);                                                                             \
    uiWindowsControl (c)->parent = parent;                                                                            \
  }

#define uiWindowsControlDefaultToplevel(type)                                                                         \
  static int type##Toplevel (uiControl *c) { return 0; }

#define uiWindowsControlDefaultVisible(type)                                                                          \
  static int type##Visible (uiControl *c) { return uiWindowsControl (c)->visible; }

#define uiWindowsControlDefaultShow(type)                                                                             \
  static void type##Show (uiControl *c)                                                                               \
  {                                                                                                                   \
    uiWindowsControl (c)->visible = 1;                                                                                \
    ShowWindow (type (c)->hwnd, SW_SHOW);                                                                             \
    uiWindowsControlNotifyVisibilityChanged (uiWindowsControl (c));                                                   \
  }

#define uiWindowsControlDefaultHide(type)                                                                             \
  static void type##Hide (uiControl *c)                                                                               \
  {                                                                                                                   \
    uiWindowsControl (c)->visible = 0;                                                                                \
    ShowWindow (type (c)->hwnd, SW_HIDE);                                                                             \
    uiWindowsControlNotifyVisibilityChanged (uiWindowsControl (c));                                                   \
  }

#define uiWindowsControlDefaultEnabled(type)                                                                          \
  static int type##Enabled (uiControl *c) { return uiWindowsControl (c)->enabled; }

#define uiWindowsControlDefaultEnable(type)                                                                           \
  static void type##Enable (uiControl *c)                                                                             \
  {                                                                                                                   \
    uiWindowsControl (c)->enabled = 1;                                                                                \
    uiWindowsControlSyncEnableState (uiWindowsControl (c), uiControlEnabledToUser (c));                               \
  }

#define uiWindowsControlDefaultDisable(type)                                                                          \
  static void type##Disable (uiControl *c)                                                                            \
  {                                                                                                                   \
    uiWindowsControl (c)->enabled = 0;                                                                                \
    uiWindowsControlSyncEnableState (uiWindowsControl (c), uiControlEnabledToUser (c));                               \
  }

#define uiWindowsControlDefaultSyncEnableState(type)                                                                  \
  static void type##SyncEnableState (uiWindowsControl *c, int enabled)                                                \
  {                                                                                                                   \
    if (uiWindowsShouldStopSyncEnableState (c, enabled))                                                              \
      return;                                                                                                         \
    EnableWindow (type (c)->hwnd, enabled);                                                                           \
  }

#define uiWindowsControlDefaultSetParentHWND(type)                                                                    \
  static void type##SetParentHWND (uiWindowsControl *c, HWND parent)                                                  \
  {                                                                                                                   \
    uiWindowsEnsureSetParentHWND (type (c)->hwnd, parent);                                                            \
  }

#define uiWindowsControlDefaultMinimumSizeChanged(type)                                                               \
  static void type##MinimumSizeChanged (uiWindowsControl *c)                                                          \
  {                                                                                                                   \
    if (uiWindowsControlTooSmall (c))                                                                                 \
      {                                                                                                               \
        uiWindowsControlContinueMinimumSizeChanged (c);                                                               \
        return;                                                                                                       \
      }                                                                                                               \
  }

#define uiWindowsControlDefaultLayoutRect(type)                                                                       \
  static void type##LayoutRect (uiWindowsControl *c, RECT *r) { uiWindowsEnsureGetWindowRect (type (c)->hwnd, r); }

#define uiWindowsControlDefaultAssignControlIDZOrder(type)                                                            \
  static void type##AssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)               \
  {                                                                                                                   \
    uiWindowsEnsureAssignControlIDZOrder (type (c)->hwnd, controlID, insertAfter);                                    \
  }

#define uiWindowsControlDefaultChildVisibilityChanged(type)                                                           \
  static void type##ChildVisibilityChanged (uiWindowsControl *c) {}

#define uiWindowsControlAllDefaultsExceptDestroy(type)                                                                \
  uiWindowsControlDefaultHandle (type);                                                                               \
  uiWindowsControlDefaultParent (type);                                                                               \
  uiWindowsControlDefaultSetParent (type);                                                                            \
  uiWindowsControlDefaultToplevel (type);                                                                             \
  uiWindowsControlDefaultVisible (type);                                                                              \
  uiWindowsControlDefaultShow (type);                                                                                 \
  uiWindowsControlDefaultHide (type);                                                                                 \
  uiWindowsControlDefaultEnabled (type);                                                                              \
  uiWindowsControlDefaultEnable (type);                                                                               \
  uiWindowsControlDefaultDisable (type);                                                                              \
  uiWindowsControlDefaultSyncEnableState (type);                                                                      \
  uiWindowsControlDefaultSetParentHWND (type);                                                                        \
  uiWindowsControlDefaultMinimumSizeChanged (type);                                                                   \
  uiWindowsControlDefaultLayoutRect (type);                                                                           \
  uiWindowsControlDefaultAssignControlIDZOrder (type);                                                                \
  uiWindowsControlDefaultChildVisibilityChanged (type)

#define uiWindowsControlAllDefaults(type)                                                                             \
  uiWindowsControlDefaultDestroy (type);                                                                              \
  uiWindowsControlAllDefaultsExceptDestroy (type)

#define uiWindowsNewControl(type, var)                                                                                  \
  var                                           = type (uiWindowsAllocControl (sizeof (type), type##Signature, #type)); \
  uiControl (var)->Destroy                      = type##Destroy;                                                        \
  uiControl (var)->Handle                       = type##Handle;                                                         \
  uiControl (var)->Parent                       = type##Parent;                                                         \
  uiControl (var)->SetParent                    = type##SetParent;                                                      \
  uiControl (var)->Toplevel                     = type##Toplevel;                                                       \
  uiControl (var)->Visible                      = type##Visible;                                                        \
  uiControl (var)->Show                         = type##Show;                                                           \
  uiControl (var)->Hide                         = type##Hide;                                                           \
  uiControl (var)->Enabled                      = type##Enabled;                                                        \
  uiControl (var)->Enable                       = type##Enable;                                                         \
  uiControl (var)->Disable                      = type##Disable;                                                        \
  uiWindowsControl (var)->SyncEnableState       = type##SyncEnableState;                                                \
  uiWindowsControl (var)->SetParentHWND         = type##SetParentHWND;                                                  \
  uiWindowsControl (var)->MinimumSize           = type##MinimumSize;                                                    \
  uiWindowsControl (var)->MinimumSizeChanged    = type##MinimumSizeChanged;                                             \
  uiWindowsControl (var)->LayoutRect            = type##LayoutRect;                                                     \
  uiWindowsControl (var)->AssignControlIDZOrder = type##AssignControlIDZOrder;                                          \
  uiWindowsControl (var)->ChildVisibilityChanged = type##ChildVisibilityChanged;                                        \
  uiWindowsControl (var)->visible                = 1;                                                                   \
  uiWindowsControl (var)->enabled                = 1;

typedef struct uiWindowsControl
{
  uiControl c;

  uiControl *parent;

  BOOL enabled;

  BOOL visible;

  void (*SyncEnableState) (uiWindowsControl *, int);

  void (*SetParentHWND) (uiWindowsControl *, HWND);

  void (*MinimumSize) (uiWindowsControl *, int *, int *);

  void (*MinimumSizeChanged) (uiWindowsControl *);

  void (*LayoutRect) (uiWindowsControl *c, RECT *r);

  void (*AssignControlIDZOrder) (uiWindowsControl *, LONG_PTR *, HWND *);

  void (*ChildVisibilityChanged) (uiWindowsControl *);
} uiWindowsControl;

typedef struct uiWindowsSizing
{
  int  BaseX;
  int  BaseY;
  LONG InternalLeading;
} uiWindowsSizing;

extern void uiWindowsControlSyncEnableState (uiWindowsControl *, int);

extern void uiWindowsControlSetParentHWND (uiWindowsControl *, HWND);

extern void uiWindowsControlMinimumSize (uiWindowsControl *, int *, int *);

extern void uiWindowsControlMinimumSizeChanged (uiWindowsControl *);

extern void uiWindowsControlLayoutRect (uiWindowsControl *, RECT *);

extern void uiWindowsControlAssignControlIDZOrder (uiWindowsControl *, LONG_PTR *, HWND *);

extern void uiWindowsControlChildVisibilityChanged (uiWindowsControl *);

extern uiWindowsControl *uiWindowsAllocControl (size_t n, uint32_t typesig, const char *typenamestr);

extern HWND uiWindowsEnsureCreateControlHWND (DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                              DWORD dwStyle, HINSTANCE hInstance, LPVOID lpParam,
                                              BOOL useStandardControlFont);

extern void uiWindowsEnsureDestroyWindow (HWND hwnd);

extern void uiWindowsEnsureSetParentHWND (HWND hwnd, HWND parent);

extern void uiWindowsEnsureAssignControlIDZOrder (HWND hwnd, LONG_PTR *controlID, HWND *insertAfter);

extern void uiWindowsEnsureGetClientRect (HWND hwnd, RECT *r);

extern char *uiWindowsWindowText (HWND hwnd);

extern void uiWindowsSetWindowText (HWND hwnd, const char *text);

extern int uiWindowsWindowTextWidth (HWND hwnd);

extern int uiWindowsWindowTextHeight (HWND hwnd);

extern void uiWindowsEnsureMoveWindowDuringResize (HWND hwnd, int x, int y, int width, int height);

extern void uiWindowsRegisterWM_COMMANDHandler (HWND hwnd, BOOL (*handler) (uiControl *, HWND, WORD, LRESULT *),
                                                uiControl *c);
extern void uiWindowsUnregisterWM_COMMANDHandler (HWND hwnd);

extern void uiWindowsRegisterWM_NOTIFYHandler (HWND hwnd, BOOL (*handler) (uiControl *, HWND, NMHDR *, LRESULT *),
                                               uiControl *c);
extern void uiWindowsUnregisterWM_NOTIFYHandler (HWND hwnd);

extern void uiWindowsRegisterWM_HSCROLLHandler (HWND hwnd, BOOL (*handler) (uiControl *, HWND, WORD, LRESULT *),
                                                uiControl *c);
extern void uiWindowsUnregisterWM_HSCROLLHandler (HWND hwnd);

extern void uiWindowsRegisterReceiveWM_WININICHANGE (HWND hwnd);

extern void uiWindowsUnregisterReceiveWM_WININICHANGE (HWND hwnd);

extern void uiWindowsGetSizing (HWND hwnd, uiWindowsSizing *sizing);

extern void uiWindowsSizingDlgUnitsToPixels (uiWindowsSizing *sizing, int *x, int *y);

extern void uiWindowsSizingStandardPadding (uiWindowsSizing *sizing, int *x, int *y);

extern HWND uiWindowsMakeContainer (uiWindowsControl *c, void (*onResize) (uiWindowsControl *));

extern BOOL uiWindowsControlTooSmall (uiWindowsControl *c);

extern void uiWindowsControlContinueMinimumSizeChanged (uiWindowsControl *c);

extern void uiWindowsControlAssignSoleControlIDZOrder (uiWindowsControl *);

extern BOOL uiWindowsShouldStopSyncEnableState (uiWindowsControl *c, int enabled);

extern void uiWindowsControlNotifyVisibilityChanged (uiWindowsControl *c);
