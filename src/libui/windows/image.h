#pragma once

#include <wincodec.h>

#include <ui/image.h>

#define abs(x)             ((x) < 0 ? -(x) : (x))
#define formatForGDI       GUID_WICPixelFormat32bppPBGRA
#define ieHRESULT(msg, hr) initerr ("=" msg, L"HRESULT", (DWORD)hr)
#define ieLastErr(msg)     initerr ("=" msg, L"GetLastError() ==", GetLastError ())

extern IWICImagingFactory *uiprivWICFactory;

extern HRESULT uiprivInitImage ();

extern void uiprivUninitImage ();

extern IWICBitmap *uiprivImageAppropriateForDC (const uiImage *i, HDC dc);

extern HRESULT uiprivWICToGDI (IWICBitmap *b, HDC dc, int width, int height, HBITMAP *hb);
