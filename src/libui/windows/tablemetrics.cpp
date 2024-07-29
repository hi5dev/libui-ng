#include <windows.h>

#include <uipriv.h>

#include "debug.h"
#include "table.h"
#include "tablemetrics.h"

#include <commctrl.h>

static HRESULT
itemRect (const HRESULT hr, const uiTable *t, const UINT uMsg, const WPARAM wParam, const LONG left, const LONG top,
          // ReSharper disable once CppDFAConstantParameter
          const LRESULT bad, RECT *r)
{
  if (hr != S_OK)
    return hr;

  ZeroMemory (r, sizeof (RECT));
  r->left = left;
  r->top  = top;

  if (SendMessageW (t->hwnd, uMsg, wParam, reinterpret_cast<LPARAM> (r)) == bad)
    {
      (void)logLastError (L"itemRect() message");
      return E_FAIL;
    }

  return S_OK;
}

HRESULT
uiprivTableGetMetrics (uiTable *t, const int iItem, const int iSubItem, uiprivTableMetrics **mout)
{
  if (mout == nullptr)
    return E_POINTER;

  auto *const m = uiprivNew (uiprivTableMetrics);

  const uiprivTableColumnParams *p = (*t->columns)[iSubItem];

  m->hasText  = static_cast<BOOL> (p->textModelColumn != -1);
  m->hasImage = static_cast<BOOL> (p->imageModelColumn != -1 || p->checkboxModelColumn != -1);

  const LRESULT state = SendMessageW (t->hwnd, LVM_GETITEMSTATE, iItem, LVIS_FOCUSED | LVIS_SELECTED);
  m->focused          = static_cast<BOOL> ((state & LVIS_FOCUSED) != 0);
  m->selected         = static_cast<BOOL> ((state & LVIS_SELECTED) != 0);

  HRESULT hr = itemRect (S_OK, t, LVM_GETITEMRECT, iItem, LVIR_BOUNDS, 0, FALSE, &(m->itemBounds));
  hr         = itemRect (hr, t, LVM_GETITEMRECT, iItem, LVIR_ICON, 0, FALSE, &(m->itemIcon));
  hr         = itemRect (hr, t, LVM_GETITEMRECT, iItem, LVIR_LABEL, 0, FALSE, &(m->itemLabel));
  hr         = itemRect (hr, t, LVM_GETSUBITEMRECT, iItem, LVIR_BOUNDS, iSubItem, 0, &(m->subitemBounds));
  hr         = itemRect (hr, t, LVM_GETSUBITEMRECT, iItem, LVIR_ICON, iSubItem, 0, &(m->subitemIcon));
  if (hr != S_OK)
    goto fail;

  m->subitemLabel      = m->subitemBounds;
  m->subitemLabel.left = m->subitemIcon.right;

  if (iSubItem == 0)
    {
      m->subitemBounds.right = m->itemLabel.right;
      m->subitemLabel.right  = m->itemLabel.right;
    }

  auto *const header = reinterpret_cast<HWND> (SendMessageW (t->hwnd, LVM_GETHEADER, 0, 0));

  m->bitmapMargin = SendMessageW (header, HDM_GETBITMAPMARGIN, 0, 0);
  if (ImageList_GetIconSize (t->imagelist, &(m->cxIcon), &(m->cyIcon)) == 0)
    {
      (void)logLastError (L"ImageList_GetIconSize()");
      hr = E_FAIL;
      goto fail;
    }

  RECT r = m->subitemLabel;
  if (m->hasText == 0 && m->hasImage == 0)
    r = m->subitemBounds;

  else if (m->hasImage == 0 && iSubItem != 0)
    r.left = m->subitemBounds.left;

  m->realTextBackground = r;
  m->realTextRect       = r;

  if (m->hasImage != 0 && iSubItem != 0)
    m->realTextRect.left += 2;

  else if (iSubItem != 0)
    m->realTextRect.left += m->bitmapMargin; // NOLINT(*-narrowing-conversions)

  *mout = m;
  return S_OK;

fail:;
  uiprivFree (m);
  *mout = nullptr;
  return hr;
}
