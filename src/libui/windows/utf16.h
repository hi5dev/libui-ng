#pragma once

#include <minwindef.h>

#define emptyUTF16() ((WCHAR *)uiprivAlloc (1 * sizeof (WCHAR), "WCHAR[]"))
#define emptyUTF8()  ((char *)uiprivAlloc (1 * sizeof (char), "char[]"))

extern WCHAR *ftoutf16 (double d);

extern WCHAR *itoutf16 (int i);

extern WCHAR *strf (const WCHAR *format, ...);

extern WCHAR *toUTF16 (const char *str);

extern WCHAR *utf16dup (const WCHAR *orig);

extern WCHAR *vstrf (const WCHAR *format, va_list ap);

extern char *toUTF8 (const WCHAR *wstr);

extern char *LFtoCRLF (const char *lfonly);

extern void CRLFtoLF (char *s);
