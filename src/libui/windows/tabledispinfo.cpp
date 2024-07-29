#include "tabledispinfo.h"

#include "table.h"
#include "utf16.h"

#include <uipriv.h>
#include <wchar.h>

static HRESULT
handleLVIF_TEXT (uiTable *t, const NMLVDISPINFOW *nm, const uiprivTableColumnParams *p)
{
  if ((nm->item.mask & LVIF_TEXT) == 0)
    return S_OK;

  int strcol = -1;
  if (p->textModelColumn != -1)
    strcol = p->textModelColumn;

  else if (p->buttonModelColumn != -1)
    strcol = p->buttonModelColumn;

  if (strcol != -1)
    {
      uiTableValue *value = uiprivTableModelCellValue (t->model, nm->item.iItem, strcol);
      WCHAR        *wstr  = toUTF16 (uiTableValueString (value));

      uiFreeTableValue (value);
      wcsncpy (nm->item.pszText, wstr, nm->item.cchTextMax);
      nm->item.pszText[nm->item.cchTextMax - 1] = L'\0';
      uiprivFree (wstr);
      return S_OK;
    }

  if (p->progressBarModelColumn != -1)
    {
      const int progress
          = uiprivTableProgress (t, nm->item.iItem, nm->item.iSubItem, p->progressBarModelColumn, nullptr);

      if (progress == -1)
        {
          (void)wcsncpy (nm->item.pszText, L"Indeterminate", nm->item.cchTextMax);
          return S_OK;
        }

      (void)_snwprintf (nm->item.pszText, nm->item.cchTextMax, L"%d%%", progress);
      return S_OK;
    }

  return S_OK;
}

HRESULT
uiprivTableHandleLVN_GETDISPINFO (uiTable *t, NMLVDISPINFOW *nm)
{
  const uiprivTableColumnParams *p = (*t->columns)[nm->item.iSubItem];

  return handleLVIF_TEXT (t, nm, p);
}
