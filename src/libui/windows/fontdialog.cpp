// ReSharper disable CppDFAConstantParameter
#include "fontdialog.h"

#include "d2dscratch.h"
#include "debug.h"
#include "init.h"
#include "resources.hpp"
#include "sizing.h"
#include "text.h"
#include "uipriv_windows.hpp"
#include "winutil.h"

#include <commctrl.h>
#include <d2d1.h>
#include <uipriv.h>
#include <wchar.h>

static LRESULT
cbAddString (const HWND cb, const WCHAR *str)
{
  const LRESULT lr = SendMessageW (cb, CB_ADDSTRING, 0, reinterpret_cast<LPARAM> (str));
  if (lr == static_cast<LRESULT> (CB_ERR) || lr == static_cast<LRESULT> (CB_ERRSPACE))
    (void)logLastError (L"error adding item to combobox");
  return lr;
}

static LRESULT
cbInsertString (const HWND cb, const WCHAR *str, const WPARAM pos)
{
  const LRESULT lr = SendMessageW (cb, CB_INSERTSTRING, pos, reinterpret_cast<LPARAM> (str));
  if (lr != static_cast<LRESULT> (pos))
    (void)logLastError (L"error inserting item to combobox");
  return lr;
}

static LRESULT
cbGetItemData (const HWND cb, const WPARAM item)
{
  const LRESULT data = SendMessageW (cb, CB_GETITEMDATA, item, 0);
  if (data == static_cast<LRESULT> (CB_ERR))
    (void)logLastError (L"error getting combobox item data for font dialog");
  return data;
}

static void
cbSetItemData (const HWND cb, const WPARAM item, const LPARAM data)
{
  if (SendMessageW (cb, CB_SETITEMDATA, item, data) == static_cast<LRESULT> (CB_ERR))
    (void)logLastError (L"error setting combobox item data");
}

static BOOL
cbGetCurSel (const HWND cb, LRESULT *sel)
{
  const LRESULT n = SendMessageW (cb, CB_GETCURSEL, 0, 0);
  if (n == static_cast<LRESULT> (CB_ERR))
    return FALSE;

  *sel = n;

  return TRUE;
}

static void
cbSetCurSel (const HWND cb, const WPARAM item)
{
  if (SendMessageW (cb, CB_SETCURSEL, item, 0) != static_cast<LRESULT> (item))
    (void)logLastError (L"error selecting combobox item");
}

static LRESULT
cbGetCount (const HWND cb)
{
  const LRESULT n = SendMessageW (cb, CB_GETCOUNT, 0, 0);
  if (n == static_cast<LRESULT> (CB_ERR))
    (void)logLastError (L"error getting combobox item count");
  return n;
}

static void
cbWipeAndReleaseData (const HWND cb)
{
  const LRESULT n = cbGetCount (cb);
  for (LRESULT i = 0; i < n; i++)
    {
      auto *obj = reinterpret_cast<IUnknown *> (cbGetItemData (cb, static_cast<WPARAM> (i)));
      obj->Release ();
    }
  SendMessageW (cb, CB_RESETCONTENT, 0, 0);
}

static WCHAR *
cbGetItemText (const HWND cb, const WPARAM item)
{
  const LRESULT len = SendMessageW (cb, CB_GETLBTEXTLEN, item, 0);
  if (len == static_cast<LRESULT> (CB_ERR))
    (void)logLastError (L"error getting item text length from combobox");

  auto *text = static_cast<WCHAR *> (uiprivAlloc ((len + 1) * sizeof (WCHAR), "WCHAR[]"));
  if (SendMessageW (cb, CB_GETLBTEXT, item, reinterpret_cast<LPARAM> (text)) != len)
    (void)logLastError (L"error getting item text from combobox");

  return text;
}

static BOOL
cbTypeToSelect (const HWND cb, LRESULT *posOut, const BOOL)
{
  DWORD selStart;
  DWORD selEnd;

  // start by saving the current selection as setting the item will change the selection
  SendMessageW (cb, CB_GETEDITSEL, reinterpret_cast<WPARAM> (&selStart), reinterpret_cast<LPARAM> (&selEnd));

  WCHAR *text = windowText (cb);

  const LRESULT pos = SendMessageW (cb, CB_FINDSTRINGEXACT, static_cast<WPARAM> (-1), reinterpret_cast<LPARAM> (text));

  if (pos == static_cast<LRESULT> (CB_ERR))
    {
      uiprivFree (text);
      return FALSE;
    }

  cbSetCurSel (cb, static_cast<WPARAM> (pos));
  if (posOut != nullptr)
    *posOut = pos;

  uiprivFree (text);

  if (SendMessageW (cb, CB_SETEDITSEL, 0, MAKELPARAM (selStart, selEnd)) != static_cast<LRESULT> (TRUE))
    (void)logLastError (L"error restoring combobox edit selection");

  return TRUE;
}

static void
wipeStylesBox (const fontDialog *f)
{
  cbWipeAndReleaseData (f->styleCombobox);
}

static WCHAR *
fontStyleName (const fontCollection *fc, IDWriteFont *font)
{
  IDWriteLocalizedStrings *str;

  const HRESULT hr = font->GetFaceNames (&str);
  if (hr != S_OK)
    (void)logHRESULT (L"error getting font style name for font dialog", hr);

  WCHAR *wstr = uiprivFontCollectionCorrectString (fc, str);
  str->Release ();

  return wstr;
}

static void
queueRedrawSampleText (const fontDialog *f)
{
  invalidateRect (f->sampleBox, nullptr, TRUE);
}

static void
styleChanged (fontDialog *f)
{
  LRESULT pos;

  const BOOL selected = cbGetCurSel (f->styleCombobox, &pos);
  if (selected == 0)
    return;
  f->curStyle = pos;

  auto *const font
      = reinterpret_cast<IDWriteFont *> (cbGetItemData (f->styleCombobox, static_cast<WPARAM> (f->curStyle)));

  f->weight  = font->GetWeight ();
  f->style   = font->GetStyle ();
  f->stretch = font->GetStretch ();

  queueRedrawSampleText (f);
}

static void
styleEdited (fontDialog *f)
{
  if (cbTypeToSelect (f->styleCombobox, &f->curStyle, FALSE) != 0)
    styleChanged (f);
}

static void
familyChanged (fontDialog *f)
{
  LRESULT      pos;
  IDWriteFont *font;
  IDWriteFont *matchFont;

  const BOOL selected = cbGetCurSel (f->familyCombobox, &pos);
  if (selected == 0)
    return;
  f->curFamily = pos;

  auto *family
      = reinterpret_cast<IDWriteFontFamily *> (cbGetItemData (f->familyCombobox, static_cast<WPARAM> (f->curFamily)));

  // for the nearest style match
  // when we select a new family, we want the nearest style to the previously selected one to be chosen
  // this is how the Choose Font sample does it
  HRESULT hr = family->GetFirstMatchingFont (f->weight, f->stretch, f->style, &matchFont);
  if (hr != S_OK)
    (void)logHRESULT (L"error finding first matching font to previous style in font dialog", hr);

  // we can't just compare pointers; a "newly created" object comes out
  // the Choose Font sample appears to do this instead
  const DWRITE_FONT_WEIGHT  weight  = matchFont->GetWeight ();
  const DWRITE_FONT_STYLE   style   = matchFont->GetStyle ();
  const DWRITE_FONT_STRETCH stretch = matchFont->GetStretch ();
  matchFont->Release ();

  // TODO test mutliple streteches; all the fonts I have have only one stretch value?
  wipeStylesBox (f);
  const UINT32 n        = family->GetFontCount ();
  UINT32       matching = 0; // a safe/suitable default just in case
  for (UINT32 i = 0; i < n; i++)
    {
      hr = family->GetFont (i, &font);
      if (hr != S_OK)
        (void)logHRESULT (L"error getting font for filling styles box", hr);
      WCHAR *label = fontStyleName (f->fc, font);
      pos          = cbAddString (f->styleCombobox, label);
      uiprivFree (label);
      cbSetItemData (f->styleCombobox, static_cast<WPARAM> (pos), reinterpret_cast<LPARAM> (font));
      if (font->GetWeight () == weight && font->GetStyle () == style && font->GetStretch () == stretch)
        matching = i;
    }

  // and now, load the match
  cbSetCurSel (f->styleCombobox, matching);
  styleChanged (f);
}

static void
familyEdited (fontDialog *f)
{
  if (cbTypeToSelect (f->familyCombobox, &f->curFamily, FALSE) != 0)
    familyChanged (f);
}

static const struct
{
  const WCHAR *text;
  double       value;
} defaultSizes[] = {
  { L"8",    8  },
  { L"9",    9  },
  { L"10",   10 },
  { L"11",   11 },
  { L"12",   12 },
  { L"14",   14 },
  { L"16",   16 },
  { L"18",   18 },
  { L"20",   20 },
  { L"22",   22 },
  { L"24",   24 },
  { L"26",   26 },
  { L"28",   28 },
  { L"36",   36 },
  { L"48",   48 },
  { L"72",   72 },
  { nullptr, 0  },
};

static void
sizeChanged (fontDialog *f)
{
  LRESULT pos;

  const BOOL selected = cbGetCurSel (f->sizeCombobox, &pos);
  if (selected == 0)
    return;

  f->curSize = defaultSizes[pos].value;
  queueRedrawSampleText (f);
}

static void
sizeEdited (fontDialog *f)
{

  // handle type-to-selection
  if (cbTypeToSelect (f->sizeCombobox, nullptr, FALSE) != 0)
    {
      sizeChanged (f);
      return;
    }
  // selection not chosen, try to parse the typing
  const WCHAR *wsize = windowText (f->sizeCombobox);
  const double size  = wcstod (wsize, nullptr);

  if (size <= 0)
    return;

  f->curSize = size;

  queueRedrawSampleText (f);
}

static void
fontDialogDrawSampleText (const fontDialog *f, ID2D1RenderTarget *rt)
{
  D2D1_COLOR_F             color;
  D2D1_BRUSH_PROPERTIES    props;
  ID2D1SolidColorBrush    *black;
  IDWriteLocalizedStrings *sampleStrings;
  BOOL                     exists;
  IDWriteTextFormat       *format;
  D2D1_RECT_F              rect;

  const WCHAR *sample;

  color.r = 0.0;
  color.g = 0.0;
  color.b = 0.0;
  color.a = 1.0;
  ZeroMemory (&props, sizeof (D2D1_BRUSH_PROPERTIES));
  props.opacity = 1.0;

  // identity matrix
  props.transform._11 = 1;
  props.transform._22 = 1;

  HRESULT hr = rt->CreateSolidColorBrush (&color, &props, &black);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating solid brush", hr);

  auto *font = reinterpret_cast<IDWriteFont *> (cbGetItemData (f->styleCombobox, static_cast<WPARAM> (f->curStyle)));

  hr = font->GetInformationalStrings (DWRITE_INFORMATIONAL_STRING_SAMPLE_TEXT, &sampleStrings, &exists);
  if (hr != S_OK)
    exists = FALSE;

  if (exists != 0)
    {
      sample = uiprivFontCollectionCorrectString (f->fc, sampleStrings);
      sampleStrings->Release ();
    }
  else
    {
      sample = L"The quick brown fox jumps over the lazy dog.";
    }

  // DirectWrite doesn't allow creating a text format from a font; we need to get this ourselves
  WCHAR *family = cbGetItemText (f->familyCombobox, f->curFamily);
  hr = dwfactory->CreateTextFormat (family, nullptr, font->GetWeight (), font->GetStyle (), font->GetStretch (),
                                    f->curSize * (96.0 / 72.0), L"", &format); // NOLINT(*-narrowing-conversions)

  if (hr != S_OK)
    (void)logHRESULT (L"error creating IDWriteTextFormat", hr);
  uiprivFree (family);

  rect.left   = 0;
  rect.top    = 0;
  rect.right  = realGetSize (rt).width;
  rect.bottom = realGetSize (rt).height;
  rt->DrawText (sample, wcslen (sample), format, &rect, black, D2D1_DRAW_TEXT_OPTIONS_NONE,
                DWRITE_MEASURING_MODE_NATURAL);

  format->Release ();
  black->Release ();
}

static LRESULT CALLBACK
fontDialogSampleSubProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam,
                         const UINT_PTR uIdSubclass, const DWORD_PTR dwRefData)
{
  switch (uMsg)
    {
    case msgD2DScratchPaint:
      {
        auto             *rt = reinterpret_cast<ID2D1RenderTarget *> (lParam);
        const fontDialog *f  = reinterpret_cast<fontDialog *> (dwRefData);
        fontDialogDrawSampleText (f, rt);
        return 0;
      }
    case WM_NCDESTROY:
      {
        if (RemoveWindowSubclass (hwnd, fontDialogSampleSubProc, uIdSubclass) == FALSE)
          (void)logLastError (L"error removing font dialog sample text subclass");
        break;
      }
    default:
      break;
    }
  return DefSubclassProc (hwnd, uMsg, wParam, lParam);
}

static void
setupInitialFontDialogState (fontDialog *f)
{
  WCHAR   wsize[512]; // this should be way more than enough
  LRESULT pos;

  (void)_snwprintf (wsize, 512, L"%g", f->params->size);

  if (SendMessageW (f->sizeCombobox, WM_SETTEXT, 0, reinterpret_cast<LPARAM> (wsize)) != static_cast<LRESULT> (TRUE))
    (void)logLastError (L"error setting size combobox to initial font size");
  sizeEdited (f);

  if (cbGetCurSel (f->sizeCombobox, &pos) != 0)
    if (SendMessageW (f->sizeCombobox, CB_SETTOPINDEX, static_cast<WPARAM> (pos), 0) != 0)
      (void)logLastError (L"error making chosen size topmost in the size combobox");

  f->weight  = f->params->font->GetWeight ();
  f->style   = f->params->font->GetStyle ();
  f->stretch = f->params->font->GetStretch ();

  if (SendMessageW (f->familyCombobox, WM_SETTEXT, 0, reinterpret_cast<LPARAM> (f->params->familyName))
      != static_cast<LRESULT> (TRUE))
    (void)logLastError (L"error setting family combobox to initial font family");

  familyEdited (f);
}

static fontDialog *
beginFontDialog (const HWND hwnd, const LPARAM lParam)
{
  UINT32             i;
  IDWriteFontFamily *family;

  auto *f           = uiprivNew (fontDialog);
  f->hwnd           = hwnd;
  f->params         = reinterpret_cast<fontDialogParams *> (lParam);
  f->familyCombobox = getDlgItem (f->hwnd, rcFontFamilyCombobox);
  f->styleCombobox  = getDlgItem (f->hwnd, rcFontStyleCombobox);
  f->sizeCombobox   = getDlgItem (f->hwnd, rcFontSizeCombobox);

  f->fc                  = uiprivLoadFontCollection ();
  const UINT32 nFamilies = f->fc->fonts->GetFontFamilyCount ();
  for (i = 0; i < nFamilies; i++)
    {
      const HRESULT hr = f->fc->fonts->GetFontFamily (i, &family);
      if (hr != S_OK)
        (void)logHRESULT (L"error getting font family", hr);
      WCHAR        *wname = uiprivFontCollectionFamilyName (f->fc, family);
      const LRESULT pos   = cbAddString (f->familyCombobox, wname);
      uiprivFree (wname);
      cbSetItemData (f->familyCombobox, static_cast<WPARAM> (pos), reinterpret_cast<LPARAM> (family));
    }

  for (i = 0; defaultSizes[i].text != nullptr; i++)
    cbInsertString (f->sizeCombobox, defaultSizes[i].text, i);

  const HWND samplePlacement = getDlgItem (f->hwnd, rcFontSamplePlacement);
  uiWindowsEnsureGetWindowRect (samplePlacement, &(f->sampleRect));
  mapWindowRect (nullptr, f->hwnd, &(f->sampleRect));
  uiWindowsEnsureDestroyWindow (samplePlacement);
  f->sampleBox = newD2DScratch (f->hwnd, &(f->sampleRect), reinterpret_cast<HMENU> (rcFontSamplePlacement),
                                fontDialogSampleSubProc, reinterpret_cast<DWORD_PTR> (f));

  setupInitialFontDialogState (f);
  return f;
}

static void
endFontDialog (fontDialog *f, const INT_PTR code)
{
  wipeStylesBox (f);
  cbWipeAndReleaseData (f->familyCombobox);
  uiprivFontCollectionFree (f->fc);
  if (EndDialog (f->hwnd, code) == 0)
    (void)logLastError (L"error ending font dialog");
  uiprivFree (f);
}

static INT_PTR
// ReSharper disable once CppDFAConstantFunctionResult
tryFinishDialog (fontDialog *f, const WPARAM wParam)
{

  // cancelling
  if (LOWORD (wParam) != IDOK)
    {
      endFontDialog (f, 1);
      return TRUE;
    }

  // OK
  uiprivDestroyFontDialogParams (f->params);
  f->params->font = reinterpret_cast<IDWriteFont *> (cbGetItemData (f->styleCombobox, f->curStyle));

  // we need to save font from being destroyed with the combobox
  f->params->font->AddRef ();
  f->params->size = f->curSize;

  auto *family = reinterpret_cast<IDWriteFontFamily *> (cbGetItemData (f->familyCombobox, f->curFamily));

  f->params->familyName = uiprivFontCollectionFamilyName (f->fc, family);
  f->params->styleName  = fontStyleName (f->fc, f->params->font);

  endFontDialog (f, 2);

  return TRUE;
}

static INT_PTR CALLBACK
fontDialogDlgProc (const HWND hwnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{

  auto *f = reinterpret_cast<fontDialog *> (GetWindowLongPtrW (hwnd, DWLP_USER));
  if (f == nullptr)
    {
      if (uMsg == WM_INITDIALOG)
        {
          f = beginFontDialog (hwnd, lParam);
          SetWindowLongPtrW (hwnd, DWLP_USER, reinterpret_cast<LONG_PTR> (f));
          return TRUE;
        }
      return FALSE;
    }

  switch (uMsg)
    {
    case WM_COMMAND:
      SetWindowLongPtrW (f->hwnd, DWLP_MSGRESULT, 0); // just in case
      switch (LOWORD (wParam))
        {
        case IDOK:
        case IDCANCEL:
          if (HIWORD (wParam) != BN_CLICKED)
            return FALSE;
          return tryFinishDialog (f, wParam);
        case rcFontFamilyCombobox:
          if (HIWORD (wParam) == CBN_SELCHANGE)
            {
              familyChanged (f);
              return TRUE;
            }
          if (HIWORD (wParam) == CBN_EDITCHANGE)
            {
              familyEdited (f);
              return TRUE;
            }
          return FALSE;
        case rcFontStyleCombobox:
          if (HIWORD (wParam) == CBN_SELCHANGE)
            {
              styleChanged (f);
              return TRUE;
            }
          if (HIWORD (wParam) == CBN_EDITCHANGE)
            {
              styleEdited (f);
              return TRUE;
            }
          return FALSE;
        case rcFontSizeCombobox:
          if (HIWORD (wParam) == CBN_SELCHANGE)
            {
              sizeChanged (f);
              return TRUE;
            }
          if (HIWORD (wParam) == CBN_EDITCHANGE)
            {
              sizeEdited (f);
              return TRUE;
            }
          return FALSE;
        default:
          break;
        }
      return FALSE;

    default:
      break;
    }
  return FALSE;
}

/*
// this is for our custom DirectWrite-based font dialog (see fontdialog.cpp)
// this is based on the "New Font Dialog with Syslink" in Microsoft's font.dlg
// LONGTERM look at localization
// LONGTERM make it look tighter and nicer like the real one, including the actual heights of the font family and
style comboboxes rcFontDialog DIALOGEX 13, 54, 243, 200 STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU |
DS_3DLOOK CAPTION "Font" FONT 9, "Segoe UI" BEGIN LTEXT		"&Font:", -1, 7, 7, 98, 9 COMBOBOX
rcFontFamilyCombobox, 7, 16, 98, 76, CBS_SIMPLE | CBS_AUTOHSCROLL | CBS_DISABLENOSCROLL | CBS_SORT | WS_VSCROLL |
WS_TABSTOP | CBS_HASSTRINGS

        LTEXT		"Font st&yle:", -1, 114, 7, 74, 9
        COMBOBOX	rcFontStyleCombobox, 114, 16, 74, 76,
                CBS_SIMPLE | CBS_AUTOHSCROLL | CBS_DISABLENOSCROLL |
                WS_VSCROLL | WS_TABSTOP | CBS_HASSTRINGS

        LTEXT		"&Size:", -1, 198, 7, 36, 9
        COMBOBOX	rcFontSizeCombobox, 198, 16, 36, 76,
                CBS_SIMPLE | CBS_AUTOHSCROLL | CBS_DISABLENOSCROLL |
                CBS_SORT | WS_VSCROLL | WS_TABSTOP | CBS_HASSTRINGS

        GROUPBOX		"Sample", -1, 7, 97, 227, 70, WS_GROUP
        CTEXT			"AaBbYyZz", rcFontSamplePlacement, 9, 106, 224, 60, SS_NOPREFIX | NOT WS_VISIBLE

        DEFPUSHBUTTON	"OK", IDOK, 141, 181, 45, 14, WS_GROUP
        PUSHBUTTON		"Cancel", IDCANCEL, 190, 181, 45, 14, WS_GROUP
END
*/
static const uint8_t data_rcFontDialog[] = {
  0x01, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC4, 0x00, 0xC8, 0x80, 0x0A, 0x00, 0x0D,
  0x00, 0x36, 0x00, 0xF3, 0x00, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x00, 0x6F, 0x00, 0x6E, 0x00, 0x74, 0x00,
  0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x01, 0x53, 0x00, 0x65, 0x00, 0x67, 0x00, 0x6F, 0x00, 0x65, 0x00, 0x20,
  0x00, 0x55, 0x00, 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50,
  0x07, 0x00, 0x07, 0x00, 0x62, 0x00, 0x09, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x00, 0x26, 0x00, 0x46,
  0x00, 0x6F, 0x00, 0x6E, 0x00, 0x74, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x41, 0x0B, 0x21, 0x50, 0x07, 0x00, 0x10, 0x00, 0x62, 0x00, 0x4C, 0x00, 0xE8, 0x03, 0x00, 0x00, 0xFF,
  0xFF, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50,
  0x72, 0x00, 0x07, 0x00, 0x4A, 0x00, 0x09, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x00, 0x46, 0x00, 0x6F,
  0x00, 0x6E, 0x00, 0x74, 0x00, 0x20, 0x00, 0x73, 0x00, 0x74, 0x00, 0x26, 0x00, 0x79, 0x00, 0x6C, 0x00, 0x65, 0x00,
  0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x0A, 0x21, 0x50, 0x72,
  0x00, 0x10, 0x00, 0x4A, 0x00, 0x4C, 0x00, 0xE9, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x50, 0xC6, 0x00, 0x07, 0x00, 0x24, 0x00, 0x09,
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x00, 0x26, 0x00, 0x53, 0x00, 0x69, 0x00, 0x7A, 0x00, 0x65, 0x00,
  0x3A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x0B, 0x21, 0x50, 0xC6,
  0x00, 0x10, 0x00, 0x24, 0x00, 0x4C, 0x00, 0xEA, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0x85, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x02, 0x50, 0x07, 0x00, 0x61, 0x00, 0xE3, 0x00, 0x46,
  0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x53, 0x00, 0x61, 0x00, 0x6D, 0x00, 0x70, 0x00, 0x6C, 0x00,
  0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x02, 0x40, 0x09,
  0x00, 0x6A, 0x00, 0xE0, 0x00, 0x3C, 0x00, 0xEB, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0x82, 0x00, 0x41, 0x00, 0x61, 0x00,
  0x42, 0x00, 0x62, 0x00, 0x59, 0x00, 0x79, 0x00, 0x5A, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x50, 0x8D, 0x00, 0xB5, 0x00, 0x2D, 0x00, 0x0E, 0x00, 0x01, 0x00,
  0x00, 0x00, 0xFF, 0xFF, 0x80, 0x00, 0x4F, 0x00, 0x4B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x50, 0xBE, 0x00, 0xB5, 0x00, 0x2D, 0x00, 0x0E, 0x00, 0x02, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0x80, 0x00, 0x43, 0x00, 0x61, 0x00, 0x6E, 0x00, 0x63, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x00,
  0x00,
};
static_assert (ARRAYSIZE (data_rcFontDialog) == 476, "wrong size for resource rcFontDialog");

BOOL
uiprivShowFontDialog (const HWND parent, fontDialogParams *params)
{
  switch (DialogBoxIndirectParamW (hInstance, reinterpret_cast<const DLGTEMPLATE *> (data_rcFontDialog), parent,
                                   fontDialogDlgProc, reinterpret_cast<LPARAM> (params)))
    {
    case 1:
      // cancel
      return FALSE;

    case 2:
      // ok
      // make the compiler happy by putting the return after the switch
      break;

    default:
      (void)logLastError (L"error running font dialog");
    }
  return TRUE;
}

static IDWriteFontFamily *
tryFindFamily (IDWriteFontCollection *fc, const WCHAR *name)
{
  UINT32             index;
  BOOL               exists;
  IDWriteFontFamily *family;

  HRESULT hr = fc->FindFamilyName (name, &index, &exists);
  if (hr != S_OK)
    (void)logHRESULT (L"error finding font family for font dialog", hr);

  if (exists == 0)
    return nullptr;

  hr = fc->GetFontFamily (index, &family);
  if (hr != S_OK)
    (void)logHRESULT (L"error extracting found font family for font dialog", hr);

  return family;
}

void
uiprivLoadInitialFontDialogParams (fontDialogParams *params)
{
  IDWriteFont *font;
  HRESULT      hr;

  // Our preferred font is Arial 10 Regular.
  // 10 comes from the official font dialog.
  // Arial Regular is a reasonable, if arbitrary, default; it's similar to the defaults on other systems.
  // If Arial isn't found, we'll use Helvetica and then MS Sans Serif as fallbacks, and if not, we'll just grab the
  // first font family in the collection.

  // We need the correct localized name for Regular (and possibly Arial too? let's say yes to be safe), so let's grab
  // the strings from DirectWrite instead of hardcoding them.
  fontCollection    *fc     = uiprivLoadFontCollection ();
  IDWriteFontFamily *family = tryFindFamily (fc->fonts, L"Arial");
  if (family == nullptr)
    {
      family = tryFindFamily (fc->fonts, L"Helvetica");
      if (family == nullptr)
        {
          family = tryFindFamily (fc->fonts, L"MS Sans Serif");
          if (family == nullptr)
            {
              hr = fc->fonts->GetFontFamily (0, &family);
              if (hr != S_OK)
                (void)logHRESULT (L"error getting first font out of font collection (worst case scenario)", hr);
            }
        }
    }

  hr = family->GetFirstMatchingFont (DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                     &font);
  if (hr != S_OK)
    (void)logHRESULT (L"error getting Regular font from Arial", hr);

  params->font       = font;
  params->size       = 10;
  params->familyName = uiprivFontCollectionFamilyName (fc, family);
  params->styleName  = fontStyleName (fc, font);

  family->Release ();
  uiprivFontCollectionFree (fc);
}

void
uiprivDestroyFontDialogParams (const fontDialogParams *params)
{
  params->font->Release ();
  uiprivFree (params->familyName);
  uiprivFree (params->styleName);
}

WCHAR *
uiprivFontDialogParamsToString (const fontDialogParams *params)
{
  auto *const text = static_cast<WCHAR *> (uiprivAlloc (512 * sizeof (WCHAR), "WCHAR[]"));

  (void)_snwprintf (text, 512, L"%s %s %g", params->familyName, params->styleName, params->size);

  return text;
}
