#include <windows.h>

#include <uipriv.h>

#include "debug.h"
#include "init.h"
#include "table.h"
#include "tableediting.h"
#include "tablemetrics.h"
#include "text.h"
#include "utf16.h"

#include <commctrl.h>

static HRESULT
resizeEdit (uiTable *t, const WCHAR *wstr, const int iItem, const int iSubItem)
{
  uiprivTableMetrics *m;

  TEXTMETRICW tm;

  SIZE textSize;

  RECT editRect;

  RECT clientRect;

  const HRESULT hr = uiprivTableGetMetrics (t, iItem, iSubItem, &m);
  if (hr != S_OK)
    return hr;

  RECT r = m->realTextRect;
  uiprivFree (m);

  const HDC   dc       = GetDC (t->hwnd);
  auto *const prevFont = static_cast<HFONT> (SelectObject (dc, hMessageFont));

  GetTextMetricsW (dc, &tm);
  GetTextExtentPoint32W (dc, wstr, wcslen (wstr), &textSize); // NOLINT(*-narrowing-conversions)
  SelectObject (dc, prevFont);
  ReleaseDC (t->hwnd, dc);

  SendMessageW (t->edit, EM_GETRECT, 0, reinterpret_cast<LPARAM> (&editRect));
  r.left -= editRect.left;

  // find the top of the text
  r.top += (r.bottom - r.top - tm.tmHeight) / 2;

  // and move THAT by the right offset
  r.top -= editRect.top;
  r.right = r.left + textSize.cx;

  // the real listview does this to add some extra space at the end
  r.right += 4 * GetSystemMetrics (SM_CXEDGE) + GetSystemMetrics (SM_CYEDGE);

  // and make the bottom equally positioned to the top
  r.bottom = r.top + editRect.top + tm.tmHeight + editRect.top;

  // make sure the edit box doesn't stretch outside the listview
  GetClientRect (t->hwnd, &clientRect);
  IntersectRect (&r, &r, &clientRect);

  static constexpr auto flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER;
  SetWindowPos (t->edit, nullptr, r.left, r.top, r.right - r.left, r.bottom - r.top, flags);

  return S_OK;
}

static LRESULT CALLBACK
editSubProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, const UINT_PTR uIDSubclass,
             const DWORD_PTR dwRefData)
{
  auto   *t = reinterpret_cast<uiTable *> (dwRefData);
  HRESULT hr;

  switch (uMsg)
    {
    case WM_KEYDOWN:
      switch (wParam)
        {
        case VK_RETURN:
          {
            hr = uiprivTableFinishEditingText (t);
            if (hr != S_OK)
              (void)logHRESULT (L"uiprivTableFinishEditingText", hr);
            return 0;
          }

        case VK_ESCAPE:
          {
            hr = uiprivTableAbortEditingText (t);
            if (hr != S_OK)
              (void)logHRESULT (L"uiprivTableAbortEditingText", hr);
            return 0;
          }

        default:
          break;
        }

      break;

    case WM_GETDLGCODE:
      return DLGC_HASSETSEL | DLGC_WANTALLKEYS;

    case WM_NCDESTROY:
      {
        if (RemoveWindowSubclass (hwnd, editSubProc, uIDSubclass) == FALSE)
          (void)logLastError (L"RemoveWindowSubclass()");
      }

    default:;
      break;
    }

  return DefSubclassProc (hwnd, uMsg, wParam, lParam);
}

static HRESULT
openEditControl (uiTable *t, const int iItem, const int iSubItem, const uiprivTableColumnParams *p)
{
  // the real list view accepts changes to the existing item when editing a new item
  HRESULT hr = uiprivTableFinishEditingText (t);
  if (hr != S_OK)
    return hr;

  // the real list view creates the edit control with the string
  uiTableValue *value = uiprivTableModelCellValue (t->model, iItem, p->textModelColumn);
  WCHAR        *wstr  = toUTF16 (uiTableValueString (value));
  uiFreeTableValue (value);

  static constexpr auto flags = WS_CHILD | WS_CLIPSIBLINGS | WS_BORDER | ES_AUTOHSCROLL;
  t->edit = CreateWindowExW (0, L"EDIT", wstr, flags, 0, 0, 16384, 16384, t->hwnd, reinterpret_cast<HMENU> (1),
                             hInstance, nullptr);
  if (t->edit == nullptr)
    {
      (void)logLastError (L"CreateWindowExW()");
      uiprivFree (wstr);
      return E_FAIL;
    }

  SendMessageW (t->edit, WM_SETFONT, reinterpret_cast<WPARAM> (hMessageFont), TRUE);
  SetWindowSubclass (t->edit, editSubProc, 0, reinterpret_cast<DWORD_PTR> (t));

  hr = resizeEdit (t, wstr, iItem, iSubItem);
  if (hr != S_OK)
    return hr;

  SetFocus (t->edit);
  ShowWindow (t->edit, SW_SHOW);
  SendMessageW (t->edit, EM_SETSEL, 0, -1);

  uiprivFree (wstr);
  t->editedItem    = iItem;
  t->editedSubitem = iSubItem;
  return S_OK;
}

HRESULT
uiprivTableResizeWhileEditing (uiTable *t)
{
  if (t->edit == nullptr)
    return S_OK;

  WCHAR *text = windowText (t->edit);

  const HRESULT hr = resizeEdit (t, text, t->editedItem, t->editedSubitem);

  uiprivFree (text);

  return hr;
}

HRESULT
uiprivTableFinishEditingText (uiTable *t)
{
  if (t->edit == nullptr)
    return S_OK;

  char *text = uiWindowsWindowText (t->edit);

  uiTableValue *value = uiNewTableValueString (text);

  uiFreeText (text);

  const uiprivTableColumnParams *p = (*t->columns)[t->editedSubitem];

  uiprivTableModelSetCellValue (t->model, t->editedItem, p->textModelColumn, value);

  uiFreeTableValue (value);

  return uiprivTableAbortEditingText (t);
}

HRESULT
uiprivTableAbortEditingText (uiTable *t)
{
  if (t->edit == nullptr)
    return S_OK;

  // set t->edit to NULL now so we don't trigger commits on focus killed
  const HWND edit = t->edit;
  t->edit         = nullptr;

  if (DestroyWindow (edit) == 0)
    {
      (void)logLastError (L"DestroyWindow()");
      return E_FAIL;
    }

  return S_OK;
}

HRESULT
uiprivTableHandleNM_CLICK (uiTable *t, const NMITEMACTIVATE *nm, LRESULT *lResult)
{
  LVHITTESTINFO            ht;
  uiprivTableColumnParams *p;

  int  modelColumn    = -1;
  int  editableColumn = -1;
  bool text           = false;
  bool checkbox       = false;

  ZeroMemory (&ht, sizeof (LVHITTESTINFO));
  ht.pt = nm->ptAction;
  if (SendMessageW (t->hwnd, LVM_SUBITEMHITTEST, 0, reinterpret_cast<LPARAM> (&ht)) == static_cast<LRESULT> (-1))
    goto done;

  p = (*t->columns)[ht.iSubItem];
  if (p->textModelColumn != -1)
    {
      modelColumn    = p->textModelColumn;
      editableColumn = p->textEditableModelColumn;
      text           = true;
    }

  else if (p->checkboxModelColumn != -1)
    {
      modelColumn    = p->checkboxModelColumn;
      editableColumn = p->checkboxEditableModelColumn;
      checkbox       = true;
    }

  else if (p->buttonModelColumn != -1)
    {
      modelColumn    = p->buttonModelColumn;
      editableColumn = p->buttonClickableModelColumn;
    }

  if (modelColumn == -1)
    goto done;

  if (text && t->inDoubleClickTimer != 0)
    goto done;

  if (uiprivTableModelCellEditable (t->model, ht.iItem, editableColumn) == 0)
    goto done;

  if (text)
    {
      const HRESULT hr = openEditControl (t, ht.iItem, ht.iSubItem, p);
      if (hr != S_OK)
        return hr;
    }

  else if (checkbox)
    {
      if ((ht.flags & LVHT_ONITEMICON) == 0)
        goto done;

      uiTableValue *value   = uiprivTableModelCellValue (t->model, ht.iItem, modelColumn);
      const int     checked = uiTableValueInt (value);
      uiFreeTableValue (value);

      value = uiNewTableValueInt (checked == 0); // NOLINT(*-implicit-bool-conversion)
      uiprivTableModelSetCellValue (t->model, ht.iItem, modelColumn, value);
      uiFreeTableValue (value);
    }

  else
    {
      uiprivTableModelSetCellValue (t->model, ht.iItem, modelColumn, nullptr);
    }

done:
  *lResult = 0;
  return S_OK;
}
