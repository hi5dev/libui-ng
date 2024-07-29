#pragma once

#include <dwrite.h>

typedef struct fontCollection
{
  IDWriteFontCollection *fonts;

  WCHAR userLocale[LOCALE_NAME_MAX_LENGTH];
  int   userLocaleSuccess;
} fontCollection;

extern IDWriteFactory *dwfactory;

extern HRESULT uiprivInitDrawText ();

extern void uiprivUninitDrawText ();

extern fontCollection *uiprivLoadFontCollection ();

extern WCHAR *uiprivFontCollectionFamilyName (const fontCollection *fc, IDWriteFontFamily *family);

extern void uiprivFontCollectionFree (fontCollection *fc);

extern WCHAR *uiprivFontCollectionCorrectString (const fontCollection *fc, IDWriteLocalizedStrings *names);
