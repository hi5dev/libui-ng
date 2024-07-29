// ReSharper disable CppDFAConstantParameter
#include "winapi.h"

#include "table.h"

#include "tabledispinfo.h"
#include "tabledraw.h"
#include "tableediting.h"
#include "tablemodel.h"

#include "debug.h"
#include "init.h"
#include "utf16.h"

#include <controlsigs.h>
#include <ui/userbugs.h>
#include <uipriv.h>

API const uiTableTextColumnOptionalParams uiprivDefaultTextColumnOptionalParams;

uiTableModel *
uiNewTableModel (uiTableModelHandler *mh)
{
  auto *m   = uiprivNew (uiTableModel);
  m->mh     = mh;
  m->tables = new std::vector<uiTable *>;
  return m;
}

void
uiFreeTableModel (uiTableModel *m)
{
  delete m->tables;
  uiprivFree (m);
}

void
uiTableModelRowInserted (const uiTableModel *m, const int newIndex)
{
  LVITEMW item  = {};
  item.mask     = 0;
  item.iItem    = newIndex;
  item.iSubItem = 0;

  for (const auto *t : *m->tables)
    {
      if (ListView_InsertItem (t->hwnd, &item) == -1)
        (void)logLastError (L"error calling ListView_InsertItem in uiTableModelRowInserted()");

      // redraw every row from the new row down to simulate adding it
      if (ListView_RedrawItems (t->hwnd, newIndex, ListView_GetItemCount (t->hwnd) - 1) == -1)
        (void)logLastError (L"error calling ListView_RedrawItems in uiTableModelRowInserted()");
    }
}

void
uiTableModelRowChanged (const uiTableModel *m, const int index)
{
  for (const auto *t : *m->tables)
    if (SendMessageW (t->hwnd, LVM_UPDATE, static_cast<WPARAM> (index), 0) == static_cast<LRESULT> (-1))
      (void)logLastError (L"error calling LVM_UPDATE in uiTableModelRowChanged()");
}

void
uiTableModelRowDeleted (const uiTableModel *m, const int oldIndex)
{
  for (const auto *t : *m->tables)
    {
      if (ListView_DeleteItem (t->hwnd, oldIndex) == -1)
        (void)logLastError (L"error calling ListView_DeleteItem() in uiTableModelRowDeleted()");

      // redraw every row from the new nth row down to simulate removing the old nth row
      if (ListView_RedrawItems (t->hwnd, oldIndex, ListView_GetItemCount (t->hwnd) - 1) == -1)
        (void)logLastError (L"error calling ListView_RedrawItems() in uiTableModelRowDeleted()");
    }
}

static void
defaultOnRowClicked (uiTable *, int, void *)
{
  // no default action
}

static void
defaultOnRowDoubleClicked (uiTable *, int, void *)
{
  // no default action
}

void
uiTableOnRowClicked (uiTable *t, void (*f) (uiTable *, int, void *), void *data)
{
  t->onRowClicked     = f;
  t->onRowClickedData = data;
}

void
uiTableOnRowDoubleClicked (uiTable *t, void (*f) (uiTable *, int, void *), void *data)
{
  t->onRowDoubleClicked     = f;
  t->onRowDoubleClickedData = data;
}

uiTableModelHandler *
uiprivTableModelHandler (const uiTableModel *m)
{
  return m->mh;
}

static LRESULT CALLBACK
tableSubProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam, const UINT_PTR uIDSubclass,
              const DWORD_PTR dwRefData)
{
  auto   *t     = reinterpret_cast<uiTable *> (dwRefData);
  auto   *nmhdr = reinterpret_cast<NMHDR *> (lParam);
  HRESULT hr;

  bool finishEdit = false;
  bool abortEdit  = false;
  switch (uMsg)
    {
    case WM_TIMER:
      {
        if (wParam == reinterpret_cast<WPARAM> (&t->inDoubleClickTimer))
          {
            t->inDoubleClickTimer = FALSE;
            KillTimer (hwnd, wParam);
            return 0;
          }
        if (wParam != reinterpret_cast<WPARAM> (t))
          break;

        for (auto &i : *t->indeterminatePositions)
          {
            i.second++;
            SendMessageW (hwnd, LVM_UPDATE, static_cast<WPARAM> (i.first.first), 0);
          }
        return 0;
      }

    case WM_LBUTTONDOWN:
      {
        t->inLButtonDown      = TRUE;
        const LRESULT lResult = DefSubclassProc (hwnd, uMsg, wParam, lParam);
        t->inLButtonDown      = FALSE;
        return lResult;
      }

    case WM_COMMAND:
      {
        if (HIWORD (wParam) == EN_UPDATE)
          {
            // the real list view resizes the edit control on this notification specifically
            hr = uiprivTableResizeWhileEditing (t);
            if (hr != S_OK)
              (void)logHRESULT (L"uiprivTableResizeWhileEditing", hr);

            break;
          }

        // the real list view accepts changes in this case
        if (HIWORD (wParam) == EN_KILLFOCUS)
          finishEdit = true;
        break;
      }
    case WM_NOTIFY:
      {
        const auto *const header = reinterpret_cast<HWND> (SendMessageW (t->hwnd, LVM_GETHEADER, 0, 0));
        if (nmhdr->hwndFrom == header)
          {
            const auto *nm = reinterpret_cast<NMHEADERW *> (nmhdr);

            switch (nmhdr->code)
              {
              case HDN_ITEMCHANGED:
                if ((nm->pitem->mask & HDI_WIDTH) == 0)
                  break;
                [[fallthrough]];

              case HDN_DIVIDERDBLCLICK:
                [[fallthrough]];

              case HDN_TRACK:
                [[fallthrough]];

              case HDN_ENDTRACK:
                finishEdit = true;
                [[fallthrough]];

              default:;
                break;
              }
          }

        if (nmhdr->code == NM_KILLFOCUS)
          finishEdit = true;
        break;
      }
    case LVM_CANCELEDITLABEL:
      {
        finishEdit = true;
        break;
      }

    case LVM_SETITEMCOUNT:
      {
        if (wParam <= t->editedItem)
          abortEdit = true;
        break;
      }

    case LVM_DELETEITEM:
      {
        if (wParam == t->editedItem)
          abortEdit = true;
        break;
      }

    case LVM_DELETEALLITEMS:
      {
        abortEdit = true;
        break;
      }

    case WM_NCDESTROY:
      {
        if (RemoveWindowSubclass (hwnd, tableSubProc, uIDSubclass) == FALSE)
          (void)logLastError (L"RemoveWindowSubclass()");
        [[fallthrough]];
      }

    default:
      break;
    }

  if (finishEdit)
    {
      hr = uiprivTableFinishEditingText (t);
      if (hr != S_OK)
        (void)logHRESULT (L"uiprivTableFinishEditingText", hr);
    }

  else if (abortEdit)
    {
      hr = uiprivTableAbortEditingText (t);
      if (hr != S_OK)
        (void)logHRESULT (L"uiprivTableFinishEditingText", hr);
    }

  return DefSubclassProc (hwnd, uMsg, wParam, lParam);
}

int
uiprivTableProgress (uiTable *t, const int item, const int subitem, const int modelColumn, LONG *pos)
{
  std::pair<int, int> p;

  bool startTimer = false;
  bool stopTimer  = false;

  uiTableValue *value    = uiprivTableModelCellValue (t->model, item, modelColumn);
  const int     progress = uiTableValueInt (value);
  uiFreeTableValue (value);

  p.first  = item;
  p.second = subitem;

  const auto iter = t->indeterminatePositions->find (p);
  if (iter == t->indeterminatePositions->end ())
    {
      if (progress == -1)
        {
          startTimer                      = t->indeterminatePositions->empty ();
          (*t->indeterminatePositions)[p] = 0;
          if (pos != NULL)
            *pos = 0;
        }
    }
  else if (progress != -1)
    {
      t->indeterminatePositions->erase (p);
      stopTimer = t->indeterminatePositions->empty ();
    }
  else if (pos != NULL)
    *pos = iter->second;

  if (startTimer && SetTimer (t->hwnd, reinterpret_cast<UINT_PTR> (t), 30, NULL) == 0)
    (void)logLastError (L"SetTimer()");

  if (stopTimer && KillTimer (t->hwnd, reinterpret_cast<UINT_PTR> (&t)) == 0)
    (void)logLastError (L"KillTimer()");

  return progress;
}

void
uiTableHeaderSetSortIndicator (const uiTable *t, const int column, const uiSortIndicator indicator)
{
  int fmt;

  if (indicator == uiSortIndicatorAscending)
    fmt = HDF_SORTUP;
  else if (indicator == uiSortIndicatorDescending)
    fmt = HDF_SORTDOWN;
  else
    fmt = 0;

  auto *lvhdr = reinterpret_cast<HWND> (SendMessageW (t->hwnd, LVM_GETHEADER, 0, 0));
  if (lvhdr != nullptr)
    {
      HDITEM hdri = {};
      hdri.mask   = HDI_FORMAT;
      if (SendMessageW (lvhdr, HDM_GETITEM, static_cast<WPARAM> (column), reinterpret_cast<LPARAM> (&hdri)) != 0)
        {
          hdri.fmt &= ~(HDF_SORTUP | HDF_SORTDOWN);
          hdri.fmt |= fmt;
          SendMessageW (lvhdr, HDM_SETITEM, static_cast<WPARAM> (column), reinterpret_cast<LPARAM> (&hdri));
        }
    }
}

uiSortIndicator
uiTableHeaderSortIndicator (const uiTable *t, const int column)
{
  auto *const lvhdr = reinterpret_cast<HWND> (SendMessageW (t->hwnd, LVM_GETHEADER, 0, 0));

  if (lvhdr != nullptr)
    {
      HDITEM hdri = {};
      hdri.mask   = HDI_FORMAT;
      if (SendMessageW (lvhdr, HDM_GETITEM, static_cast<WPARAM> (column), reinterpret_cast<LPARAM> (&hdri)) != 0)
        {
          if ((hdri.fmt & HDF_SORTUP) != 0)
            return uiSortIndicatorAscending;
          if ((hdri.fmt & HDF_SORTDOWN) == 0)
            return uiSortIndicatorDescending;
        }
    }
  return uiSortIndicatorNone;
}

void
uiTableHeaderOnClicked (uiTable *t, void (*f) (uiTable *table, int column, void *data), void *data)
{
  t->headerOnClicked     = f;
  t->headerOnClickedData = data;
}

static void
defaultHeaderOnClicked (uiTable *, int, void *)
{
  // do nothing
}

void
uiTableOnSelectionChanged (uiTable *t, void (*f) (uiTable *table, void *data), void *data)
{
  t->onSelectionChanged     = f;
  t->onSelectionChangedData = data;
}

static void
defaultOnSelectionChanged (uiTable *table, void *data)
{
  // do nothing
}

uiTableSelection *
uiTableGetSelection (const uiTable *t)
{
  unsigned    i    = 0;
  int         iPos = -1;
  auto *const s    = uiprivNew (uiTableSelection);

  s->NumRows = ListView_GetSelectedCount (t->hwnd);
  s->Rows    = NULL;

  if (s->NumRows > 0)
    s->Rows = static_cast<int *> (uiprivAlloc (s->NumRows * sizeof (*s->Rows), "uiTableSelection->Rows"));

  while ((iPos = ListView_GetNextItem (t->hwnd, iPos, LVNI_SELECTED)) != -1)
    s->Rows[i++] = iPos;

  return s;
}

void
uiTableSetSelection (const uiTable *t, const uiTableSelection *sel)
{

  if ((t->selectionMode == uiTableSelectionModeNone && sel->NumRows > 0)
      || (t->selectionMode == uiTableSelectionModeZeroOrOne && sel->NumRows > 1)
      || (t->selectionMode == uiTableSelectionModeOne && sel->NumRows > 1))
    {
      // TODO log error
      return;
    }

  /* clear selection */
  ListView_SetItemState (t->hwnd, -1, 0, LVIS_SELECTED);

  for (int i = 0; i < sel->NumRows; ++i)
    ListView_SetItemState (t->hwnd, sel->Rows[i], LVIS_SELECTED, LVIS_SELECTED);
}

void
_uiTableSignalOnSelectionChanged (uiTable *t)
{
  if (t->maskOnSelectionChanged == 0)
    t->onSelectionChanged (t, t->onSelectionChangedData);
}

// TODO properly integrate compound statements
static BOOL
onWM_NOTIFY (uiControl *c, HWND, NMHDR *nmhdr, LRESULT *lResult)
{
  auto   *t = uiTable (c);
  HRESULT hr;

  switch (nmhdr->code)
    {
    case LVN_GETDISPINFO:
      hr = uiprivTableHandleLVN_GETDISPINFO (t, reinterpret_cast<NMLVDISPINFOW *> (nmhdr), lResult);
      if (hr != S_OK)
        {
          // TODO
          return FALSE;
        }
      return TRUE;
    case NM_CUSTOMDRAW:
      hr = uiprivTableHandleNM_CUSTOMDRAW (t, reinterpret_cast<NMLVCUSTOMDRAW *> (nmhdr), lResult);
      if (hr != S_OK)
        {
          // TODO
          return FALSE;
        }
      return TRUE;
    case NM_CLICK:
      {
        LVHITTESTINFO ht = {};
        ht.pt            = reinterpret_cast<NMITEMACTIVATE *> (nmhdr)->ptAction;
        if (SendMessageW (t->hwnd, LVM_SUBITEMHITTEST, 0, reinterpret_cast<LPARAM> (&ht)) == -1)
          return FALSE;
        (*t->onRowClicked) (t, ht.iItem, t->onRowClickedData);

        // Handle editing
        hr = uiprivTableHandleNM_CLICK (t, reinterpret_cast<NMITEMACTIVATE *> (nmhdr), lResult);
        if (hr != S_OK)
          {
            (void)logHRESULT (L"uiprivTableHandleNM_CLICK", hr);
            return FALSE;
          }
        return TRUE;
      }

    case NM_DBLCLK:
      {
        LVHITTESTINFO ht = {};
        ht.pt            = reinterpret_cast<NMITEMACTIVATE *> (nmhdr)->ptAction;
        if (SendMessageW (t->hwnd, LVM_SUBITEMHITTEST, 0, reinterpret_cast<LPARAM> (&ht)) == -1)
          return FALSE;
        (*t->onRowDoubleClicked) (t, ht.iItem, t->onRowDoubleClickedData);
        return TRUE;
      }
    case LVN_ITEMCHANGED:
      {
        auto *nm = reinterpret_cast<NMLISTVIEW *> (nmhdr);
        UINT  oldFocused;
        UINT  newFocused;
        UINT  oldSelected;
        UINT  newSelected;
        int   nSelected;

        oldSelected = nm->uOldState & LVIS_SELECTED;
        newSelected = nm->uNewState & LVIS_SELECTED;
        oldFocused  = nm->uOldState & LVIS_FOCUSED;
        newFocused  = nm->uNewState & LVIS_FOCUSED;

        switch (t->selectionMode)
          {
          case uiTableSelectionModeNone:
            if (newSelected != 0 || newFocused != 0)
              ListView_SetItemState (t->hwnd, nm->iItem, 0, LVIS_SELECTED | LVIS_FOCUSED);
            break;
          case uiTableSelectionModeOne:
            // Ignore deselect all
            if (nm->iItem == -1)
              break;
            // Prevent deselection via CTRL+SPACE (win32 bug)
            if (oldSelected != 0 && newSelected == 0)
              {
                t->maskOnSelectionChanged = TRUE;
                ListView_SetItemState (t->hwnd, nm->iItem, LVIS_SELECTED, LVIS_SELECTED);
                t->maskOnSelectionChanged = FALSE;
              }
            // Signal selection change on selection of a new item
            if (nm->iItem != t->lastFocusedItem && oldSelected == 0 && newSelected != 0)
              _uiTableSignalOnSelectionChanged (t);

            t->lastFocusedItem           = nm->iItem;
            t->lastFocusedItemIsSelected = TRUE;
            break;
          case uiTableSelectionModeZeroOrOne:
            // Set focused item to be selected
            if (oldFocused == 0 && newFocused != 0)
              {
                t->lastFocusedItem           = nm->iItem;
                t->lastFocusedItemIsSelected = FALSE;
                ListView_SetItemState (t->hwnd, -1, 0, LVIS_SELECTED);
                ListView_SetItemState (t->hwnd, nm->iItem, LVIS_SELECTED, LVIS_SELECTED);
                break;
              }
            /* Ignore CTRL+SHIFT+SPACE on the focused item if selected
             * as it immediately does a reselect again */
            if (nm->iItem == t->lastFocusedItem && t->lastFocusedItemIsSelected != 0
                && HIBYTE (GetKeyState (VK_CONTROL)) && HIBYTE (GetKeyState (VK_SHIFT))
                && HIBYTE (GetKeyState (VK_SPACE)))
              break;
            if (nm->iItem == t->lastFocusedItem && t->lastFocusedItemIsSelected != 0 && oldSelected != 0
                && newSelected == 0)
              {
                t->lastFocusedItemIsSelected = FALSE;
                _uiTableSignalOnSelectionChanged (t);
              }
            if (nm->iItem == t->lastFocusedItem && t->lastFocusedItemIsSelected == 0 && oldSelected == 0
                && newSelected != 0)
              {
                t->lastFocusedItemIsSelected = TRUE;
                _uiTableSignalOnSelectionChanged (t);
              }
            break;
          case uiTableSelectionModeZeroOrMany:
            nSelected = ListView_GetSelectedCount (t->hwnd);
            // Ignore deselect all
            if (nm->iItem == -1)
              break;
            /* CTRL+SHIFT+SPACE on the focused item */
            if (nm->iItem == t->lastFocusedItem && HIBYTE (GetKeyState (VK_CONTROL)) && HIBYTE (GetKeyState (VK_SHIFT))
                && HIBYTE (GetKeyState (VK_SPACE)))
              {
                /* Do not signal deselect as it immediately
                 * does a reselect again */
                if (t->lastFocusedItemIsSelected != 0)
                  {
                    /* Deselect the focused item if others are still
                     * selected to trigger a signal later on */
                    if (oldSelected != 0 && newSelected == 0 && ListView_GetSelectedCount (t->hwnd) > 0)
                      t->lastFocusedItemIsSelected = FALSE;
                    break;
                  }
                /* Do not signal select if multiple items are still
                 * selected, as these will still get deselect later
                 * on in the sequence */
                if (t->lastFocusedItemIsSelected == 0 && newSelected != 0 && ListView_GetSelectedCount (t->hwnd) > 1)
                  break;
              }
            /* Do not signal select of a selected item unless
             * the selection count has changed (other items
             * have been deselected) */
            if (nm->iItem == t->lastFocusedItem && t->lastFocusedItemIsSelected != 0 && newSelected != 0
                && nSelected == t->lastNumSelected)
              break;

            // Single item de/select
            if ((oldSelected == 0 && newSelected != 0) || (oldSelected != 0 && newSelected == 0) ||
                // SHIFT multi select
                (oldFocused == 0 && newFocused != 0 && HIBYTE (GetKeyState (VK_SHIFT))
                 && nm->iItem != ListView_GetSelectionMark (t->hwnd)))
              _uiTableSignalOnSelectionChanged (t);

            if (oldFocused == 0 && newFocused != 0)
              t->lastFocusedItem = nm->iItem;
            if (nm->iItem == t->lastFocusedItem)
              t->lastFocusedItemIsSelected
                  = ListView_GetItemState (t->hwnd, t->lastFocusedItem, LVIS_SELECTED) & LVIS_SELECTED;
            t->lastNumSelected = nSelected;

            break;
          }

        // TODO clean up these if cases
        if (t->inLButtonDown == 0 && t->edit == NULL)
          return FALSE;

        if (t->inLButtonDown != 0 && oldFocused == 0 && newFocused != 0)
          {
            t->inDoubleClickTimer = TRUE;
            // TODO check error
            SetTimer (t->hwnd, reinterpret_cast<UINT_PTR> (&t->inDoubleClickTimer), GetDoubleClickTime (), NULL);
            *lResult = 0;
            return TRUE;
          }
        // the nm->iItem == -1 case is because that is used if "the change has been applied to all items in the list
        // view"
        if (t->edit != NULL && oldFocused != 0 && newFocused == 0 && (t->editedItem == nm->iItem || nm->iItem == -1))
          {
            HRESULT hresult = uiprivTableFinishEditingText (t);
            if (hresult != S_OK)
              {
                (void)logHRESULT (L"uiprivTableFinishEditingText", hresult);
                return FALSE;
              }
            *lResult = 0;
            return TRUE;
          }
        return FALSE;
      }
    case LVN_ODSTATECHANGED:
      {
        auto *nm = reinterpret_cast<NMLVODSTATECHANGE *> (nmhdr);
        int   nSelected;

        switch (t->selectionMode)
          {
          case uiTableSelectionModeZeroOrOne:
            // Windows somehow decides to ignore the first request
            ListView_SetItemState (t->hwnd, -1, 0, LVIS_SELECTED);
            ListView_SetItemState (t->hwnd, -1, 0, LVIS_SELECTED);
            break;
          case uiTableSelectionModeZeroOrMany:
            nSelected = ListView_GetSelectedCount (t->hwnd);
            /* Signal selection change on SHIFT+CLICK or SHIFT+SPACE
             * when doing so on an item that has been focused via
             * CTRL+UP or CTRL+DOWN */
            if (nSelected != t->lastNumSelected
                && ((nm->iFrom == t->lastFocusedItem && nm->iTo == ListView_GetSelectionMark (t->hwnd))
                    || (nm->iTo == t->lastFocusedItem && nm->iFrom == ListView_GetSelectionMark (t->hwnd))))
              {
                t->lastFocusedItemIsSelected = TRUE;
                _uiTableSignalOnSelectionChanged (t);
              }
            t->lastNumSelected = nSelected;
            break;

          default:
            break;
          }
        return TRUE;
      }
    case LVN_COLUMNCLICK:
      {
        auto *nm = reinterpret_cast<NMLISTVIEW *> (nmhdr);

        hr = uiprivTableFinishEditingText (t);
        if (hr != S_OK)
          {
            // TODO
            return FALSE;
          }
        t->headerOnClicked (t, nm->iSubItem, t->headerOnClickedData);
        return TRUE;
      }
    // the real list view accepts changes when scrolling or clicking column headers
    case LVN_BEGINSCROLL:
      hr = uiprivTableFinishEditingText (t);
      if (hr != S_OK)
        {
          // TODO
          return FALSE;
        }
      *lResult = 0;
      return TRUE;
    default:
      break;
    }
  return FALSE;
}

static void
uiTableDestroy (uiControl *c)
{
  auto *const t = uiTable (c);

  const uiTableModel *model = t->model;

  const HRESULT hr = uiprivTableAbortEditingText (t);
  if (hr != S_OK)
    (void)logHRESULT (L"uiprivTableAbortEditingText", hr);

  uiWindowsUnregisterWM_NOTIFYHandler (t->hwnd);
  uiWindowsEnsureDestroyWindow (t->hwnd);
  // detach table from model
  for (auto it = model->tables->begin (); it != model->tables->end (); ++it)
    {
      if (*it == t)
        {
          model->tables->erase (it);
          break;
        }
    }
  // free the columns
  for (auto *col : *t->columns)
    uiprivFree (col);

  delete t->columns;
  delete t->indeterminatePositions;

  uiFreeControl (uiControl (t));
}

uiWindowsControlAllDefaultsExceptDestroy (uiTable);

#define tableMinWidth  107      /* in line with other controls */
#define tableMinHeight (14 * 3) /* header + 2 lines (roughly) */

static void
uiTableMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  uiTable        *t = uiTable (c);
  uiWindowsSizing sizing;
  int             x;
  int             y;

  x = tableMinWidth;
  y = tableMinHeight;
  uiWindowsGetSizing (t->hwnd, &sizing);
  uiWindowsSizingDlgUnitsToPixels (&sizing, &x, &y);
  *width  = x;
  *height = y;
}

static uiprivTableColumnParams *
appendColumn (uiTable *t, const char *name, const int colfmt)
{
  LVCOLUMNW lvc;

  ZeroMemory (&lvc, sizeof (LVCOLUMNW));
  lvc.mask    = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
  lvc.fmt     = colfmt;
  lvc.cx      = 120; // TODO
  WCHAR *wstr = toUTF16 (name);
  lvc.pszText = wstr;
  if (SendMessageW (t->hwnd, LVM_INSERTCOLUMNW, t->nColumns, reinterpret_cast<LPARAM> (&lvc))
      == static_cast<LRESULT> (-1))
    (void)logLastError (L"error calling LVM_INSERTCOLUMNW in appendColumn()");
  uiprivFree (wstr);
  t->nColumns++;

  auto *p = uiprivNew (uiprivTableColumnParams);

  p->textModelColumn             = -1;
  p->textEditableModelColumn     = -1;
  p->textParams                  = uiprivDefaultTextColumnOptionalParams;
  p->imageModelColumn            = -1;
  p->checkboxModelColumn         = -1;
  p->checkboxEditableModelColumn = -1;
  p->progressBarModelColumn      = -1;
  p->buttonModelColumn           = -1;
  t->columns->push_back (p);
  return p;
}

void
uiTableAppendTextColumn (uiTable *t, const char *name, const int textModelColumn, const int textEditableModelColumn,
                         const uiTableTextColumnOptionalParams *textParams)
{

  uiprivTableColumnParams *p = appendColumn (t, name, LVCFMT_LEFT);
  p->textModelColumn         = textModelColumn;
  p->textEditableModelColumn = textEditableModelColumn;
  if (textParams != NULL)
    p->textParams = *textParams;
}

void
uiTableAppendImageColumn (uiTable *t, const char *name, const int imageModelColumn)
{

  uiprivTableColumnParams *p = appendColumn (t, name, LVCFMT_LEFT);
  p->imageModelColumn        = imageModelColumn;
}

void
uiTableAppendImageTextColumn (uiTable *t, const char *name, const int imageModelColumn, const int textModelColumn,
                              const int textEditableModelColumn, const uiTableTextColumnOptionalParams *textParams)
{

  uiprivTableColumnParams *p = appendColumn (t, name, LVCFMT_LEFT);
  p->textModelColumn         = textModelColumn;
  p->textEditableModelColumn = textEditableModelColumn;
  if (textParams != NULL)
    p->textParams = *textParams;
  p->imageModelColumn = imageModelColumn;
}

void
uiTableAppendCheckboxColumn (uiTable *t, const char *name, const int checkboxModelColumn,
                             const int checkboxEditableModelColumn)
{

  uiprivTableColumnParams *p     = appendColumn (t, name, LVCFMT_LEFT);
  p->checkboxModelColumn         = checkboxModelColumn;
  p->checkboxEditableModelColumn = checkboxEditableModelColumn;
}

void
uiTableAppendCheckboxTextColumn (uiTable *t, const char *name, const int checkboxModelColumn,
                                 const int checkboxEditableModelColumn, const int textModelColumn,
                                 const int textEditableModelColumn, const uiTableTextColumnOptionalParams *textParams)
{

  uiprivTableColumnParams *p = appendColumn (t, name, LVCFMT_LEFT);
  p->textModelColumn         = textModelColumn;
  p->textEditableModelColumn = textEditableModelColumn;
  if (textParams != NULL)
    p->textParams = *textParams;
  p->checkboxModelColumn         = checkboxModelColumn;
  p->checkboxEditableModelColumn = checkboxEditableModelColumn;
}

void
uiTableAppendProgressBarColumn (uiTable *t, const char *name, const int progressModelColumn)
{

  uiprivTableColumnParams *p = appendColumn (t, name, LVCFMT_LEFT);
  p->progressBarModelColumn  = progressModelColumn;
}

void
uiTableAppendButtonColumn (uiTable *t, const char *name, const int buttonModelColumn,
                           const int buttonClickableModelColumn)
{

  // TODO see if we can get rid of this parameter
  uiprivTableColumnParams *p    = appendColumn (t, name, LVCFMT_LEFT);
  p->buttonModelColumn          = buttonModelColumn;
  p->buttonClickableModelColumn = buttonClickableModelColumn;
}

int
uiTableHeaderVisible (uiTable *t)
{
  const HWND header = static_cast<HWND> (SendMessageW (t->hwnd, LVM_GETHEADER, 0, 0));
  if (header != nullptr)
    {
      LONG style = GetWindowLong (header, GWL_STYLE);
      return (style & HDS_HIDDEN) == 0; // NOLINT(*-implicit-bool-conversion)
    }
  uiprivImplBug ("window handle %p unknown error from send LVM_GETHEADER", t->hwnd);
  return 0;
}

void
uiTableHeaderSetVisible (uiTable *t, const int visible)
{
  LONG style = GetWindowLong (t->hwnd, GWL_STYLE);
  if (visible != 0)
    SetWindowLong (t->hwnd, GWL_STYLE, style & ~LVS_NOCOLUMNHEADER);
  else
    SetWindowLong (t->hwnd, GWL_STYLE, style | LVS_NOCOLUMNHEADER);
}

uiTableSelectionMode
uiTableGetSelectionMode (uiTable *t)
{
  return t->selectionMode;
}

void
uiTableSetSelectionMode (uiTable *t, uiTableSelectionMode mode)
{
  LONG style = GetWindowLong (t->hwnd, GWL_STYLE);

  t->selectionMode = mode;

  switch (t->selectionMode)
    {
    case uiTableSelectionModeOne:
    case uiTableSelectionModeZeroOrOne:
      if (ListView_GetSelectedCount (t->hwnd) == 1)
        {
          // Ensure cached variables and focus are set up correctly
          t->lastFocusedItem           = ListView_GetNextItem (t->hwnd, -1, LVNI_SELECTED);
          t->lastFocusedItemIsSelected = TRUE;
          t->lastNumSelected           = 1;
          t->maskOnSelectionChanged    = TRUE;
          ListView_SetItemState (t->hwnd, t->lastFocusedItem, LVIS_FOCUSED, LVIS_FOCUSED);
          t->maskOnSelectionChanged = FALSE;
          break;
        }
      // Fall through
    case uiTableSelectionModeNone:
      // Reset the table to it's initial state
      ListView_SetItemState (t->hwnd, -1, 0, LVIS_SELECTED | LVIS_FOCUSED);
      // Ensure we do not match deselect all (-1)
      t->lastFocusedItem           = -2;
      t->lastFocusedItemIsSelected = FALSE;
      t->lastNumSelected           = 0;
      break;
    case uiTableSelectionModeZeroOrMany:
      t->lastNumSelected = ListView_GetSelectedCount (t->hwnd);
      break;
    default:
      uiprivUserBug ("Invalid table selection mode %d", mode);
      return;
    }

  switch (t->selectionMode)
    {
    case uiTableSelectionModeNone:
      [[fallthrough]];

    case uiTableSelectionModeOne:
      SetWindowLong (t->hwnd, GWL_STYLE, style | LVS_SINGLESEL);
      break;

    case uiTableSelectionModeZeroOrOne:
      [[fallthrough]];

    case uiTableSelectionModeZeroOrMany:
      SetWindowLong (t->hwnd, GWL_STYLE, style & ~LVS_SINGLESEL);
      break;

    default:
      break;
    }
}

uiTable *
uiNewTable (uiTableParams *p)
{
  uiTable *t;

  uiWindowsNewControl (uiTable, t);

  t->columns          = new std::vector<uiprivTableColumnParams *>;
  t->model            = p->Model;
  t->backgroundColumn = p->RowBackgroundColorModelColumn;
  uiTableHeaderOnClicked (t, defaultHeaderOnClicked, NULL);
  uiTableOnSelectionChanged (t, defaultOnSelectionChanged, NULL);

  // WS_CLIPCHILDREN is here to prevent drawing over the edit box used for editing text
  t->hwnd = uiWindowsEnsureCreateControlHWND (
      WS_EX_CLIENTEDGE, WC_LISTVIEW, L"",
      LVS_REPORT | LVS_OWNERDATA | WS_CLIPCHILDREN | WS_TABSTOP | WS_HSCROLL | WS_VSCROLL, hInstance, NULL, TRUE);
  t->model->tables->push_back (t);

  uiWindowsRegisterWM_NOTIFYHandler (t->hwnd, onWM_NOTIFY, uiControl (t));

  SendMessageW (t->hwnd, LVM_SETEXTENDEDLISTVIEWSTYLE,
                static_cast<WPARAM> ((LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_SUBITEMIMAGES)),
                static_cast<LPARAM> ((LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP | LVS_EX_SUBITEMIMAGES)));
  int n = uiprivTableModelNumRows (t->model);
  if (SendMessageW (t->hwnd, LVM_SETITEMCOUNT, static_cast<WPARAM> (n), 0) == 0)
    (void)logLastError (L"error calling LVM_SETITEMCOUNT in uiNewTable()");

  HRESULT hr = uiprivUpdateImageListSize (t);
  if (hr != S_OK)
    {
      // TODO
    }

  t->indeterminatePositions = new std::map<std::pair<int, int>, LONG>;
  if (SetWindowSubclass (t->hwnd, tableSubProc, 0, static_cast<DWORD_PTR> (t)) == FALSE)
    (void)logLastError (L"SetWindowSubclass()");

  uiTableOnRowClicked (t, defaultOnRowClicked, NULL);
  uiTableOnRowDoubleClicked (t, defaultOnRowDoubleClicked, NULL);
  uiTableSetSelectionMode (t, uiTableSelectionModeZeroOrOne);

  return t;
}

int
uiTableColumnWidth (uiTable *t, const int column)
{
  return SendMessageW (t->hwnd, LVM_GETCOLUMNWIDTH, static_cast<WPARAM> (column), // NOLINT(*-narrowing-conversions)
                       0);
}

void
uiTableColumnSetWidth (uiTable *t, const int column, int width)
{
  if (width == -1)
    width = LVSCW_AUTOSIZE_USEHEADER;

  SendMessageW (t->hwnd, LVM_SETCOLUMNWIDTH, static_cast<WPARAM> (column), static_cast<LPARAM> (width));
}
