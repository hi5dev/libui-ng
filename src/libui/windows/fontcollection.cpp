#include "fontcollection.h"

#include "debug.h"

#include <uipriv.h>

IDWriteFactory *dwfactory = NULL;

HRESULT
uiprivInitDrawText ()
{
  return DWriteCreateFactory (DWRITE_FACTORY_TYPE_SHARED, __uuidof (IDWriteFactory),
                              reinterpret_cast<IUnknown **> (&dwfactory));
}

void
uiprivUninitDrawText ()
{
  dwfactory->Release ();
}

fontCollection *
uiprivLoadFontCollection ()
{
  const auto fc = uiprivNew (fontCollection);

  const HRESULT hr = dwfactory->GetSystemFontCollection (&fc->fonts, TRUE);
  if (hr != S_OK)
    (void)logHRESULT (L"error getting system font collection", hr);

  fc->userLocaleSuccess = GetUserDefaultLocaleName (fc->userLocale, LOCALE_NAME_MAX_LENGTH);

  return fc;
}

void
uiprivFontCollectionFree (fontCollection *fc)
{
  fc->fonts->Release ();

  uiprivFree (fc);
}

WCHAR *
uiprivFontCollectionFamilyName (const fontCollection *fc, IDWriteFontFamily *family)
{
  IDWriteLocalizedStrings *names;

  const HRESULT hr = family->GetFamilyNames (&names);
  if (hr != S_OK)
    (void)logHRESULT (L"error getting names of font out", hr);

  WCHAR *str = uiprivFontCollectionCorrectString (fc, names);

  names->Release ();

  return str;
}

WCHAR *
uiprivFontCollectionCorrectString (const fontCollection *fc, IDWriteLocalizedStrings *names)
{
  BOOL   exists = FALSE;
  UINT32 length = 0;

  // FindLocaleName returns S_OK and sets index to UINT_MAX when the locale isn't found
  HRESULT hr    = S_OK;
  UINT32  index = UINT_MAX;
  if (fc->userLocaleSuccess != 0)
    hr = names->FindLocaleName (fc->userLocale, &index, &exists);

  if (hr == S_OK && index == UINT_MAX)
    hr = names->FindLocaleName (L"en-us", &index, &exists);

  if (hr != S_OK)
    {
      (void)logHRESULT (L"error getting locale name", hr);
      return nullptr;
    }

  if (exists == 0)
    index = 0;

  hr = names->GetStringLength (index, &length);
  if (hr != S_OK)
    (void)logHRESULT (L"error getting length of font name", hr);

  hr = names->GetStringLength (index, &length);
  if (hr != S_OK)
    (void)logHRESULT (L"error getting length of font name", hr);

  const auto wname = static_cast<WCHAR *> (uiprivAlloc ((length + 1) * sizeof (WCHAR), "WCHAR[]"));

  hr = names->GetString (index, wname, length + 1);
  if (hr != S_OK)
    {
      (void)logHRESULT (L"error getting font name", hr);
      return nullptr;
    }

  return wname;
}
