#include "winutil.h"

#include <commctrl.h>

#include "tab.h"

#include "debug.h"
#include "init.h"
#include "tabpage.h"
#include "utf16.h"
#include "winpublic.h"

#include <controlsigs.h>
#include <uipriv.h>

static LRESULT
curpage (const uiTab *t)
{
  return SendMessageW (t->tabHWND, TCM_GETCURSEL, 0, 0);
}

static tabPage *
tabPage (const uiTab *t, const int i)
{
  return (*t->pages)[i];
}

static void
tabPageRect (const uiTab *t, RECT *r)
{
  // this rect needs to be in parent window coordinates, but TCM_ADJUSTRECT wants a window rect, which is screen
  // coordinates because we have each page as a sibling of the tab, use the tab's own rect as the input rect
  uiWindowsEnsureGetWindowRect (t->tabHWND, r);

  SendMessageW (t->tabHWND, TCM_ADJUSTRECT, FALSE, reinterpret_cast<LPARAM> (r));

  // and get it in terms of the container instead of the screen
  mapWindowRect (nullptr, t->hwnd, r);
}

static void
tabRelayout (const uiTab *t)
{
  RECT     r;
  LONG_PTR controlID;
  HWND     insertAfter;

  // first move the tab control itself
  uiWindowsEnsureGetClientRect (t->hwnd, &r);
  uiWindowsEnsureMoveWindowDuringResize (t->tabHWND, r.left, r.top, r.right - r.left, r.bottom - r.top);

  // then the current page
  if (t->pages->empty ())
    return;

  const struct tabPage *page = tabPage (t, curpage (t)); // NOLINT(*-narrowing-conversions)

  tabPageRect (t, &r);

  controlID   = 100;
  insertAfter = nullptr;

  uiWindowsEnsureMoveWindowDuringResize (page->hwnd, r.left, r.top, r.right - r.left, r.bottom - r.top);
  uiWindowsEnsureAssignControlIDZOrder (page->hwnd, &controlID, &insertAfter);
}

static void
showHidePage (uiTab *t, const LRESULT which, const int hide)
{
  if (which == static_cast<LRESULT> (-1))
    return;

  const struct tabPage *page = tabPage (t, which); // NOLINT(*-narrowing-conversions)

  if (hide != 0)
    {
      ShowWindow (page->hwnd, SW_HIDE);
    }

  else
    {
      ShowWindow (page->hwnd, SW_SHOW);
      uiWindowsControlMinimumSizeChanged (uiWindowsControl (t));
    }
}

static BOOL
// ReSharper disable once CppParameterMayBeConstPtrOrRef
onWM_NOTIFY (uiControl *c, HWND, NMHDR *nm, LRESULT *lResult)
{
  auto *t = uiTab (c);

  if (nm->code != TCN_SELCHANGING && nm->code != TCN_SELCHANGE)
    return FALSE;

  showHidePage (t, curpage (t), nm->code == TCN_SELCHANGING); // NOLINT(*-implicit-bool-conversion)

  *lResult = 0;
  if (nm->code == TCN_SELCHANGING)
    *lResult = FALSE;

  return TRUE;
}

static void
uiTabDestroy (uiControl *c)
{
  auto *const t = uiTab (c);

  for (struct tabPage *&page : *t->pages)
    {
      uiControl *child = page->child;
      tabPageDestroy (page);
      if (child != nullptr)
        {
          uiControlSetParent (child, nullptr);
          uiControlDestroy (child);
        }
    }
  delete t->pages;
  uiWindowsUnregisterWM_NOTIFYHandler (t->tabHWND);
  uiWindowsEnsureDestroyWindow (t->tabHWND);
  uiWindowsEnsureDestroyWindow (t->hwnd);
  uiFreeControl (uiControl (t));
}

static uintptr_t
uiTabHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiTab *> (c)->hwnd);
}

static uiControl *
uiTabParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiTabSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiTabToplevel (uiControl *)
{
  return 0;
}

static int
uiTabVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiTabShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiTab *> (c)->hwnd, 5);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiTabHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiTab *> (c)->hwnd, 0);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiTabEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiTabEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiTabDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiTabSyncEnableState (uiWindowsControl *c, const int enabled)
{
  auto *t = uiTab (c);

  if (uiWindowsShouldStopSyncEnableState (uiWindowsControl (t), enabled) != 0)
    return;

  EnableWindow (t->tabHWND, enabled);
  for (struct tabPage *&page : *t->pages)
    if (page->child != nullptr)
      uiWindowsControlSyncEnableState (uiWindowsControl (page->child), enabled);
}

static void
uiTabSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiTab *> (c)->hwnd, parent);
}

static void
uiTabMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  const uiTab *t = uiTab (c);

  int pagewid;
  int pageht;

  RECT r;

  // only consider the current page
  pagewid = 0;
  pageht  = 0;
  if (!t->pages->empty ())
    {
      const struct tabPage *page = tabPage (t, curpage (t)); // NOLINT(*-narrowing-conversions)
      tabPageMinimumSize (page, &pagewid, &pageht);
    }

  r.left   = 0;
  r.top    = 0;
  r.right  = pagewid;
  r.bottom = pageht;

  // this also includes the tabs themselves
  SendMessageW (t->tabHWND, TCM_ADJUSTRECT, TRUE, reinterpret_cast<LPARAM> (&r));
  *width  = r.right - r.left;
  *height = r.bottom - r.top;
}

static void
uiTabMinimumSizeChanged (uiWindowsControl *c)
{
  auto *t = uiTab (c);

  if (uiWindowsControlTooSmall (uiWindowsControl (t)) != 0)
    {
      uiWindowsControlContinueMinimumSizeChanged (uiWindowsControl (t));
      return;
    }

  tabRelayout (t);
}

static void
uiTabLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiTab *> (c)->hwnd, r);
}

static void
uiTabAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiTab *> (c)->hwnd, controlID, insertAfter);
}

static void
uiTabChildVisibilityChanged (uiWindowsControl *c)
{
  uiWindowsControlMinimumSizeChanged (c);
}

static void
tabArrangePages (const uiTab *t)
{
  LONG_PTR controlID   = 100;
  HWND     insertAfter = nullptr;

  uiWindowsEnsureAssignControlIDZOrder (t->tabHWND, &controlID, &insertAfter);

  for (struct tabPage *&page : *t->pages)
    uiWindowsEnsureAssignControlIDZOrder (page->hwnd, &controlID, &insertAfter);
}

void
uiTabAppend (uiTab *t, const char *name, uiControl *c)
{
  uiTabInsertAt (t, name, t->pages->size (), c);
}

void
uiTabInsertAt (uiTab *t, const char *name, const int index, uiControl *c)
{
  TCITEMW item;

  const LRESULT hide = curpage (t);

  if (c != nullptr)
    uiControlSetParent (c, uiControl (t));

  struct tabPage *page = newTabPage (c);
  uiWindowsEnsureSetParentHWND (page->hwnd, t->hwnd);
  t->pages->insert (t->pages->begin () + index, page);
  tabArrangePages (t);

  ZeroMemory (&item, sizeof (TCITEMW));
  item.mask    = TCIF_TEXT;
  WCHAR *wname = toUTF16 (name);
  item.pszText = wname;

  if (SendMessageW (t->tabHWND, TCM_INSERTITEM, static_cast<WPARAM> (index), reinterpret_cast<LPARAM> (&item))
      == static_cast<LRESULT> (-1))
    (void)logLastError (L"error adding tab to uiTab");
  uiprivFree (wname);

  // we need to do this because adding the first tab doesn't send a TCN_SELCHANGE; it just shows the page
  const LRESULT show = curpage (t);
  if (show != hide)
    {
      showHidePage (t, hide, 1);
      showHidePage (t, show, 0);
    }
}

void
uiTabDelete (const uiTab *t, const int index)
{
  if (SendMessageW (t->tabHWND, TCM_DELETEITEM, static_cast<WPARAM> (index), 0) == FALSE)
    (void)logLastError (L"error deleting uiTab tab");

  struct tabPage *page = tabPage (t, index);
  if (page->child != nullptr)
    uiControlSetParent (page->child, nullptr);

  tabPageDestroy (page);
  t->pages->erase (t->pages->begin () + index);
}

int
uiTabNumPages (const uiTab *t)
{
  return t->pages->size (); // NOLINT(*-narrowing-conversions)
}

int
uiTabMargined (const uiTab *t, const int n)
{
  return tabPage (t, n)->margined;
}

void
uiTabSetMargined (uiTab *t, const int index, const int margined)
{

  struct tabPage *page = tabPage (t, index);

  page->margined = margined;

  uiWindowsControlMinimumSizeChanged (uiWindowsControl (t));
}

static void
onResize (uiWindowsControl *c)
{
  tabRelayout (uiTab (c));
}

uiTab *
uiNewTab ()
{
  auto *const t = reinterpret_cast<uiTab *> (uiWindowsAllocControl (sizeof (uiTab), uiTabSignature, "uiTab"));

  auto *control      = reinterpret_cast<uiControl *> (t);
  control->Destroy   = uiTabDestroy;
  control->Handle    = uiTabHandle;
  control->Parent    = uiTabParent;
  control->SetParent = uiTabSetParent;
  control->Toplevel  = uiTabToplevel;
  control->Visible   = uiTabVisible;
  control->Show      = uiTabShow;
  control->Hide      = uiTabHide;
  control->Enabled   = uiTabEnabled;
  control->Enable    = uiTabEnable;
  control->Disable   = uiTabDisable;

  auto *windows_control                   = uiWindowsControl (t);
  windows_control->SyncEnableState        = uiTabSyncEnableState;
  windows_control->SetParentHWND          = uiTabSetParentHWND;
  windows_control->MinimumSize            = uiTabMinimumSize;
  windows_control->MinimumSizeChanged     = uiTabMinimumSizeChanged;
  windows_control->LayoutRect             = uiTabLayoutRect;
  windows_control->AssignControlIDZOrder  = uiTabAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiTabChildVisibilityChanged;
  windows_control->visible                = 1;
  windows_control->enabled                = 1;

  t->hwnd = uiWindowsMakeContainer (uiWindowsControl (t), onResize);

  t->tabHWND
      = uiWindowsEnsureCreateControlHWND (0, WC_TABCONTROLW, L"", TCS_TOOLTIPS | WS_TABSTOP, hInstance, nullptr, TRUE);

  uiWindowsEnsureSetParentHWND (t->tabHWND, t->hwnd);

  uiWindowsRegisterWM_NOTIFYHandler (t->tabHWND, onWM_NOTIFY, uiControl (t));

  t->pages = new std::vector<struct tabPage *>;

  return t;
}
