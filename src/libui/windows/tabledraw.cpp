#include <windows.h>

#include <shobjidl.h>
#include <uxtheme.h>
#include <vsstyle.h>

#include <uipriv.h>

#include <algorithm>

#include "debug.h"
#include "table.h"
#include "tabledraw.h"

#include "image.h"
#include "tablemetrics.h"
#include "utf16.h"

#define indeterminateSegments 8

struct drawState
{
  uiTable *t;

  uiTableModel *model;

  uiprivTableColumnParams *p;

  HDC dc;

  int iItem;

  int iSubItem;

  uiprivTableMetrics *m;

  COLORREF bgColor;

  HBRUSH bgBrush;

  BOOL freeBgBrush;

  COLORREF textColor;

  HBRUSH textBrush;

  BOOL freeTextBrush;
};

static HRESULT
drawBackgrounds (const drawState *s)
{
  if (s->m->hasImage != 0 && FillRect (s->dc, &s->m->subitemIcon, GetSysColorBrush (COLOR_WINDOW)) == 0)
    {
      (void)logLastError (L"FillRect() icon");
      return E_FAIL;
    }

  if (FillRect (s->dc, &s->m->realTextBackground, s->bgBrush) == 0)
    {
      (void)logLastError (L"FillRect()");
      return E_FAIL;
    }

  return S_OK;
}

static void
centerImageRect (RECT *image, const RECT *space)
{
  // first make sure both have the same upper-left
  LONG xoff = image->left - space->left;
  LONG yoff = image->top - space->top;
  image->left -= xoff;
  image->top -= yoff;
  image->right -= xoff;
  image->bottom -= yoff;

  // now center
  xoff = (space->right - space->left - (image->right - image->left)) / 2;
  yoff = (space->bottom - space->top - (image->bottom - image->top)) / 2;
  image->left += xoff;
  image->top += yoff;
  image->right += xoff;
  image->bottom += yoff;
}

static HRESULT
drawImagePart (HRESULT hr, const drawState *s)
{
  HBITMAP b;
  RECT    r;

  if (hr != S_OK)
    return hr;
  if (s->p->imageModelColumn == -1)
    return S_OK;

  uiTableValue *value = uiprivTableModelCellValue (s->model, s->iItem, s->p->imageModelColumn);
  IWICBitmap   *wb    = uiprivImageAppropriateForDC (uiTableValueImage (value), s->dc);
  uiFreeTableValue (value);

  hr = uiprivWICToGDI (wb, s->dc, s->m->cxIcon, s->m->cyIcon, &b);
  if (hr != S_OK)
    return hr;

  if (ImageList_GetImageCount (s->t->imagelist) > 1)
    {
      if (ImageList_Replace (s->t->imagelist, 0, b, nullptr) == 0)
        {
          (void)logLastError (L"ImageList_Replace()");
          return E_FAIL;
        }
    }

  else if (ImageList_Add (s->t->imagelist, b, nullptr) == -1)
    {
      (void)logLastError (L"ImageList_Add()");
      return E_FAIL;
    }

  DeleteObject (b);

  r        = s->m->subitemIcon;
  r.right  = r.left + s->m->cxIcon;
  r.bottom = r.top + s->m->cyIcon;

  centerImageRect (&r, &s->m->subitemIcon);
  UINT fStyle = ILD_NORMAL;

  if (s->m->selected != 0)
    fStyle = ILD_SELECTED;

  if (ImageList_Draw (s->t->imagelist, 0, s->dc, r.left, r.top, fStyle) == 0)
    {
      (void)logLastError (L"ImageList_Draw()");
      return E_FAIL;
    }

  return S_OK;
}

static HRESULT
drawUnthemedCheckbox (const drawState *s, const int checked, const int enabled)
{
  RECT r;
  r = s->m->subitemIcon;

  // this is what the actual list view LVS_EX_CHECKBOXES code does to size the checkboxes
  r.right  = r.left + GetSystemMetrics (SM_CXSMICON);
  r.bottom = r.top + GetSystemMetrics (SM_CYSMICON);
  if (InflateRect (&r, -GetSystemMetrics (SM_CXEDGE), -GetSystemMetrics (SM_CYEDGE)) == 0)
    {
      (void)logLastError (L"InflateRect()");
      return E_FAIL;
    }

  r.right++;
  r.bottom++;

  centerImageRect (&r, &s->m->subitemIcon);
  UINT state = DFCS_BUTTONCHECK | DFCS_FLAT;
  if (checked != 0)
    state |= DFCS_CHECKED;

  if (enabled == 0)
    state |= DFCS_INACTIVE;

  if (DrawFrameControl (s->dc, &r, DFC_BUTTON, state) == 0)
    {
      (void)logLastError (L"DrawFrameControl()");
      return E_FAIL;
    }

  return S_OK;
}

static HRESULT
drawThemedCheckbox (const drawState *s, const HTHEME theme, const int checked, const int enabled)
{
  RECT r;
  SIZE size;
  int  state;

  HRESULT hr = GetThemePartSize (theme, s->dc, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, nullptr, TS_DRAW, &size);
  if (hr != S_OK)
    {
      (void)logHRESULT (L"GetThemePartSize()", hr);
      return hr;
    }

  r        = s->m->subitemIcon;
  r.right  = r.left + size.cx;
  r.bottom = r.top + size.cy;

  centerImageRect (&r, &s->m->subitemIcon);

  if (checked == 0 && enabled != 0)
    state = CBS_UNCHECKEDNORMAL;

  else if (checked != 0 && enabled != 0)
    state = CBS_CHECKEDNORMAL;

  else if (checked == 0)
    state = CBS_UNCHECKEDDISABLED;

  else
    state = CBS_CHECKEDDISABLED;

  hr = DrawThemeBackground (theme, s->dc, BP_CHECKBOX, state, &r, nullptr);
  if (hr != S_OK)
    (void)logHRESULT (L"DrawThemeBackground()", hr);

  return hr;
}

static HRESULT
drawCheckboxPart (HRESULT hr, const drawState *s)
{
  if (hr != S_OK)
    return hr;

  if (s->p->checkboxModelColumn == -1)
    return S_OK;

  uiTableValue *value   = uiprivTableModelCellValue (s->model, s->iItem, s->p->checkboxModelColumn);
  const int     checked = uiTableValueInt (value);

  uiFreeTableValue (value);
  const int enabled = uiprivTableModelCellEditable (s->model, s->iItem, s->p->checkboxEditableModelColumn);

  const HTHEME theme = OpenThemeData (s->t->hwnd, L"button");
  if (theme != nullptr)
    {
      hr = drawThemedCheckbox (s, theme, checked, enabled);
      if (hr != S_OK)
        return hr;

      hr = CloseThemeData (theme);
      if (hr != S_OK)
        {
          (void)logHRESULT (L"CloseThemeData()", hr);
          return hr;
        }
    }

  else
    {
      hr = drawUnthemedCheckbox (s, checked, enabled);
      if (hr != S_OK)
        return hr;
    }

  return S_OK;
}

static HRESULT
drawTextPart (const HRESULT hr, const drawState *s)
{
  if (hr != S_OK)
    return hr;

  if (s->m->hasText == 0)
    return S_OK;

  // don't draw the text underneath an edit control
  if (s->t->edit != nullptr && s->t->editedItem == s->iItem && s->t->editedSubitem == s->iSubItem)
    return S_OK;

  const COLORREF prevText = SetTextColor (s->dc, s->textColor);
  if (prevText == CLR_INVALID)
    {
      (void)logLastError (L"SetTextColor()");
      return E_FAIL;
    }

  const int prevMode = SetBkMode (s->dc, TRANSPARENT);
  if (prevMode == 0)
    {
      (void)logLastError (L"SetBkMode()");
      return E_FAIL;
    }

  uiTableValue *value = uiprivTableModelCellValue (s->model, s->iItem, s->p->textModelColumn);
  WCHAR        *wstr  = toUTF16 (uiTableValueString (value));
  uiFreeTableValue (value);

  static constexpr auto flags = DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX | DT_EDITCONTROL;
  if (DrawTextW (s->dc, wstr, -1, &s->m->realTextRect, flags) == 0)
    {
      uiprivFree (wstr);
      (void)logLastError (L"DrawTextW()");
      return E_FAIL;
    }
  uiprivFree (wstr);

  if (SetBkMode (s->dc, prevMode) != TRANSPARENT)
    {
      (void)logLastError (L"SetBkMode() prev");
      return E_FAIL;
    }

  if (SetTextColor (s->dc, prevText) != s->textColor)
    {
      (void)logLastError (L"SetTextColor() prev");
      return E_FAIL;
    }

  return S_OK;
}

static HRESULT
drawProgressBarPart (HRESULT hr, const drawState *s)
{
  LONG indeterminatePos;
  RECT rBorder;
  RECT rFill[2];

  TEXTMETRICW tm;

  int sysColor = 0;

  if (hr != S_OK)
    return hr;

  if (s->p->progressBarModelColumn == -1)
    return S_OK;

  const int progress
      = uiprivTableProgress (s->t, s->iItem, s->iSubItem, s->p->progressBarModelColumn, &indeterminatePos);

  const HTHEME theme = OpenThemeData (s->t->hwnd, L"PROGRESS");

  if (GetTextMetricsW (s->dc, &tm) == 0)
    {
      (void)logLastError (L"GetTextMetricsW()");
      hr = E_FAIL;
      goto fail;
    }

  RECT r = s->m->subitemBounds;

  // this sets the height of the progressbar and vertically centers it in one fell swoop
  r.top += (r.bottom - tm.tmHeight - r.top) / 2;
  r.bottom = r.top + tm.tmHeight;

  rBorder = r;
  InflateRect (&rBorder, -1, -1);
  if (theme != nullptr)
    {
      RECT crect;

      hr = GetThemeBackgroundContentRect (theme, s->dc, PP_TRANSPARENTBAR, PBBS_NORMAL, &rBorder, &crect);
      if (hr != S_OK)
        {
          (void)logHRESULT (L"GetThemeBackgroundContentRect()", hr);
          goto fail;
        }
      hr = DrawThemeBackground (theme, s->dc, PP_TRANSPARENTBAR, PBBS_NORMAL, &crect, nullptr);
      if (hr != S_OK)
        {
          (void)logHRESULT (L"DrawThemeBackground() border", hr);
          goto fail;
        }
    }
  else
    {
      sysColor = COLOR_HIGHLIGHT;
      if (s->m->selected != 0)
        sysColor = COLOR_HIGHLIGHTTEXT;

      const HPEN  pen       = CreatePen (PS_SOLID, 1, GetSysColor (sysColor));
      auto *const prevPen   = static_cast<HPEN> (SelectObject (s->dc, pen));
      auto *const brush     = static_cast<HBRUSH> (GetStockObject (NULL_BRUSH));
      auto *const prevBrush = static_cast<HBRUSH> (SelectObject (s->dc, brush));

      Rectangle (s->dc, rBorder.left, rBorder.top, rBorder.right, rBorder.bottom);
      SelectObject (s->dc, prevBrush);
      SelectObject (s->dc, prevPen);
      DeleteObject (pen);
    }

  int nFill = 1;
  rFill[0]  = r;

  InflateRect (&rFill[0], -1, -1);

  if (progress != -1)
    {
      rFill[0].right -= (rFill[0].right - rFill[0].left) * (100 - progress) / 100;
    }

  else
    {
      rFill[1] = rFill[0];

      const LONG barWidth   = rFill[0].right - rFill[0].left;
      const LONG pieceWidth = barWidth / indeterminateSegments;

      rFill[0].left += indeterminatePos % barWidth;
      if (rFill[0].left + pieceWidth >= rFill[0].right)
        {
          nFill++;
          rFill[1].right = rFill[1].left + (pieceWidth - (rFill[0].right - rFill[0].left));
        }

      else
        {
          rFill[0].right = rFill[0].left + pieceWidth;
        }
    }

  for (int i = 0; i < nFill; i++)
    if (theme != nullptr)
      {
        hr = DrawThemeBackground (theme, s->dc, PP_FILL, PBFS_NORMAL, &rFill[i], nullptr);
        if (hr != S_OK)
          {
            (void)logHRESULT (L"DrawThemeBackground() fill", hr);
            goto fail;
          }
      }
    else
      FillRect (s->dc, &rFill[i], GetSysColorBrush (sysColor));

  hr = S_OK;

fail:;
  if (theme != nullptr)
    (void)CloseThemeData (theme);

  return hr;
}

static HRESULT
drawButtonPart (HRESULT hr, const drawState *s)
{
  RECT r;

  TEXTMETRICW tm;

  if (hr != S_OK)
    return hr;

  if (s->p->buttonModelColumn == -1)
    return S_OK;

  uiTableValue *value = uiprivTableModelCellValue (s->model, s->iItem, s->p->buttonModelColumn);
  WCHAR        *wstr  = toUTF16 (uiTableValueString (value));
  uiFreeTableValue (value);

  const bool enabled = uiprivTableModelCellEditable (s->model, s->iItem, s->p->buttonClickableModelColumn) != 0;

  const HTHEME theme = OpenThemeData (s->t->hwnd, L"button");

  if (GetTextMetricsW (s->dc, &tm) == 0)
    {
      (void)logLastError (L"GetTextMetricsW()");
      hr = E_FAIL;
      goto fail;
    }

  r = s->m->subitemBounds;

  if (theme != nullptr)
    {

      int state = PBS_NORMAL;
      if (!enabled)
        state = PBS_DISABLED;
      hr = DrawThemeBackground (theme, s->dc, BP_PUSHBUTTON, state, &r, nullptr);
      if (hr != S_OK)
        {
          (void)logHRESULT (L"DrawThemeBackground()", hr);
          goto fail;
        }

      static constexpr auto flags = DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX;
      hr                          = DrawThemeText (theme, s->dc, BP_PUSHBUTTON, state, wstr, -1, flags, 0, &r);
      if (hr != S_OK)
        {
          (void)logHRESULT (L"DrawThemeText()", hr);
          goto fail;
        }
    }
  else
    {
      InflateRect (&r, -1, -1);

      UINT state = DFCS_BUTTONPUSH;
      if (!enabled)
        state |= DFCS_INACTIVE;

      if (DrawFrameControl (s->dc, &r, DFC_BUTTON, state) == 0)
        {
          (void)logLastError (L"DrawFrameControl()");
          hr = E_FAIL;
          goto fail;
        }

      const HBRUSH color      = GetSysColorBrush (COLOR_BTNTEXT);
      auto *const  prevColor  = static_cast<HBRUSH> (SelectObject (s->dc, color));
      const int    prevBkMode = SetBkMode (s->dc, TRANSPARENT);

      static constexpr auto flags = DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX;
      if (DrawTextW (s->dc, wstr, -1, &r, flags) == 0)
        {
          (void)logLastError (L"DrawTextW()");
          hr = E_FAIL;
          goto fail;
        }

      SetBkMode (s->dc, prevBkMode);
      SelectObject (s->dc, prevColor);
    }

  hr = S_OK;

fail:;
  if (theme != nullptr)
    (void)CloseThemeData (theme);

  uiprivFree (wstr);

  return hr;
}

static HRESULT
freeDrawState (drawState *s)
{

  HRESULT hrret = S_OK;

  if (s->m != nullptr)
    {
      uiprivFree (s->m);
      s->m = nullptr;
    }

  if (s->freeTextBrush != 0)
    {
      if (DeleteObject (s->textBrush) == 0)
        {
          (void)logLastError (L"DeleteObject()");
          hrret = E_FAIL;
        }

      s->freeTextBrush = FALSE;
    }
  if (s->freeBgBrush != 0)
    {
      if (DeleteObject (s->bgBrush) == 0)
        {
          (void)logLastError (L"DeleteObject()");
          hrret = E_FAIL;
        }

      s->freeBgBrush = FALSE;
    }

  return hrret;
}

static COLORREF
blend (const COLORREF base, const double r, const double g, const double b, const double a)
{
  double br = static_cast<double> (GetRValue (base)) / 255.0;
  double bg = static_cast<double> (GetGValue (base)) / 255.0;
  double bb = static_cast<double> (GetBValue (base)) / 255.0;

  br = r * a + br * (1.0 - a);
  bg = g * a + bg * (1.0 - a);
  bb = b * a + bb * (1.0 - a);

  return RGB (static_cast<BYTE> (br * 255), static_cast<BYTE> (bg * 255), static_cast<BYTE> (bb * 255));
}

static HRESULT
fillDrawState (drawState *s, uiTable *t, const NMLVCUSTOMDRAW *nm, uiprivTableColumnParams *p)
{
  ZeroMemory (s, sizeof (drawState));

  s->t        = t;
  s->model    = t->model;
  s->p        = p;
  s->dc       = nm->nmcd.hdc;
  s->iItem    = nm->nmcd.dwItemSpec; // NOLINT(*-narrowing-conversions)
  s->iSubItem = nm->iSubItem;

  HRESULT hr = uiprivTableGetMetrics (t, s->iItem, s->iSubItem, &s->m);
  if (hr != S_OK)
    goto fail;

  if (s->m->selected != 0)
    {
      s->bgColor   = GetSysColor (COLOR_HIGHLIGHT);
      s->bgBrush   = GetSysColorBrush (COLOR_HIGHLIGHT);
      s->textColor = GetSysColor (COLOR_HIGHLIGHTTEXT);
      s->textBrush = GetSysColorBrush (COLOR_HIGHLIGHTTEXT);
    }

  else
    {
      double r;
      double g;
      double b;
      double a;

      s->bgColor = GetSysColor (COLOR_WINDOW);
      s->bgBrush = GetSysColorBrush (COLOR_WINDOW);

      if (uiprivTableModelColorIfProvided (s->model, s->iItem, t->backgroundColumn, &r, &g, &b, &a) != 0)
        {
          s->bgColor = blend (s->bgColor, r, g, b, a);
          s->bgBrush = CreateSolidBrush (s->bgColor);
          if (s->bgBrush == nullptr)
            {
              (void)logLastError (L"CreateSolidBrush()");
              hr = E_FAIL;
              goto fail;
            }
          s->freeBgBrush = TRUE;
        }

      s->textColor = GetSysColor (COLOR_WINDOWTEXT);
      s->textBrush = GetSysColorBrush (COLOR_WINDOWTEXT);

      if (uiprivTableModelColorIfProvided (s->model, s->iItem, p->textParams.ColorModelColumn, &r, &g, &b, &a) != 0)
        {
          s->textColor = blend (s->bgColor, r, g, b, a);
          s->textBrush = CreateSolidBrush (s->textColor);
          if (s->textBrush == nullptr)
            {
              (void)logLastError (L"CreateSolidBrush()");
              hr = E_FAIL;
              goto fail;
            }
          s->freeTextBrush = TRUE;
        }
    }

  return S_OK;

fail:;
  (void)freeDrawState (s);
  return hr;
}

static HRESULT
updateAndDrawFocusRects (const HRESULT hr, const uiTable *t, const HDC dc, const int iItem,
                         const RECT *realTextBackground, RECT *focus, bool *first)
{
  if (hr != S_OK)
    return hr;

  if (GetFocus () != t->hwnd)
    return S_OK;

  const LRESULT state = SendMessageW (t->hwnd, LVM_GETITEMSTATE, static_cast<WPARAM> (iItem), LVIS_FOCUSED);
  if ((state & LVIS_FOCUSED) == 0)
    return S_OK;

  if (realTextBackground != nullptr)
    {
      if (*first)
        {
          *focus = *realTextBackground;
          *first = false;
          return S_OK;
        }

      if (focus->right == realTextBackground->left)
        {
          focus->right = realTextBackground->right;
          return S_OK;
        }
    }

  if (DrawFocusRect (dc, focus) == 0)
    {
      (void)logLastError (L"DrawFocusRect()");
      return E_FAIL;
    }

  if (realTextBackground != nullptr)
    *focus = *realTextBackground;

  return S_OK;
}

HRESULT
uiprivTableHandleNM_CUSTOMDRAW (uiTable *t, const NMLVCUSTOMDRAW *nm, LRESULT *lResult)
{
  drawState s;

  NMLVCUSTOMDRAW b;

  RECT focus;

  bool focusFirst;

  HRESULT hr = S_OK;

  switch (nm->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
      *lResult = CDRF_NOTIFYITEMDRAW;
      return S_OK;

    case CDDS_ITEMPREPAINT:
      break;

    default:
      *lResult = CDRF_DODEFAULT;
      return S_OK;
    }

  const size_t n = t->columns->size ();

  b = *nm;

  focusFirst = true;

  for (int i = 0; i < n; i++)
    {
      b.iSubItem = i;

      uiprivTableColumnParams *p = (*t->columns)[i];

      hr = fillDrawState (&s, t, &b, p);
      if (hr != S_OK)
        return hr;

      hr = drawBackgrounds (&s);
      hr = drawImagePart (hr, &s);
      hr = drawCheckboxPart (hr, &s);
      hr = drawTextPart (hr, &s);
      hr = drawProgressBarPart (hr, &s);
      hr = drawButtonPart (hr, &s);
      hr = updateAndDrawFocusRects (hr, s.t, s.dc, nm->nmcd.dwItemSpec, // NOLINT(*-narrowing-conversions)
                                    &s.m->realTextBackground, &focus, &focusFirst);
      if (hr != S_OK)
        goto fail;

      hr = freeDrawState (&s);
      if (hr != S_OK)
        return hr;
    }

  hr = updateAndDrawFocusRects (hr, t, nm->nmcd.hdc, nm->nmcd.dwItemSpec, // NOLINT(*-narrowing-conversions)
                                nullptr, &focus, &focusFirst);

  if (hr != S_OK)
    return hr;

  *lResult = CDRF_SKIPDEFAULT;
  return S_OK;

fail:;
  (void)freeDrawState (&s);
  return hr;
}

HRESULT
uiprivUpdateImageListSize (uiTable *t)
{
  SIZE sizeCheck;

  const HDC dc = GetDC (t->hwnd);
  if (dc == nullptr)
    {
      (void)logLastError (L"GetDC()");
      return E_FAIL;
    }

  int cxList   = GetSystemMetrics (SM_CXSMICON);
  int cyList   = GetSystemMetrics (SM_CYSMICON);
  sizeCheck.cx = cxList;
  sizeCheck.cy = cyList;

  const HTHEME theme = OpenThemeData (t->hwnd, L"button");
  if (theme != nullptr)
    {
      HRESULT hr = GetThemePartSize (theme, dc, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, nullptr, TS_DRAW, &sizeCheck);
      if (hr != S_OK)
        {
          (void)logHRESULT (L"GetThemePartSize()", hr);
          return hr;
        }

      cxList = std::max<LONG> (cxList, sizeCheck.cx);
      cyList = std::max<LONG> (cyList, sizeCheck.cy);

      hr = CloseThemeData (theme);
      if (hr != S_OK)
        {
          (void)logHRESULT (L"CloseThemeData()", hr);
          return hr;
        }
    }

  t->imagelist = ImageList_Create (cxList, cyList, ILC_COLOR32, 1, 1);
  if (t->imagelist == nullptr)
    {
      (void)logLastError (L"ImageList_Create()");
      return E_FAIL;
    }

  SendMessageW (t->hwnd, LVM_SETIMAGELIST, LVSIL_SMALL, reinterpret_cast<LPARAM> (t->imagelist));

  if (ReleaseDC (t->hwnd, dc) == 0)
    {
      (void)logLastError (L"ReleaseDC()");
      return E_FAIL;
    }

  return S_OK;
}
