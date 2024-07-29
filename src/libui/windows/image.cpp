#include "image.h"

#include "debug.h"

#include <cstdint>
#include <uipriv.h>
#include <vector>

IWICImagingFactory *uiprivWICFactory = nullptr;

HRESULT
uiprivInitImage ()
{
  return CoCreateInstance (CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
                           reinterpret_cast<void **> (&uiprivWICFactory));
}

void
uiprivUninitImage ()
{
  uiprivWICFactory->Release ();
  uiprivWICFactory = nullptr;
}

struct uiImage
{
  double                     width;
  double                     height;
  std::vector<IWICBitmap *> *bitmaps;
};

uiImage *
uiNewImage (const double width, const double height)
{
  auto *const i = uiprivNew (uiImage);
  i->width      = width;
  i->height     = height;
  i->bitmaps    = new std::vector<IWICBitmap *>;
  return i;
}

void
uiFreeImage (uiImage *i)
{
  for (IWICBitmap *b : *(i->bitmaps))
    b->Release ();
  delete i->bitmaps;
  uiprivFree (i);
}

void
uiImageAppend (const uiImage *i, void *pixels, const int pixelWidth, const int pixelHeight, const int byteStride)
{
  IWICBitmap     *b;
  WICRect         r;
  IWICBitmapLock *l;

  BYTE *dipp;
  UINT  size;
  UINT  realStride;

  HRESULT hr = uiprivWICFactory->CreateBitmap (pixelWidth, pixelHeight, formatForGDI, WICBitmapCacheOnDemand, &b);
  if (hr != S_OK)
    (void)logHRESULT (L"error calling CreateBitmap() in uiImageAppend()", hr);

  r.X      = 0;
  r.Y      = 0;
  r.Width  = pixelWidth;
  r.Height = pixelHeight;

  hr = b->Lock (&r, WICBitmapLockWrite, &l);
  if (hr != S_OK)
    (void)logHRESULT (L"error calling Lock() in uiImageAppend()", hr);

  const auto *pix = static_cast<uint8_t *> (pixels);

  hr = l->GetDataPointer (&size, &dipp);
  if (hr != S_OK)
    (void)logHRESULT (L"error calling GetDataPointer() in uiImageAppend()", hr);

  auto *data = dipp;

  hr = l->GetStride (&realStride);
  if (hr != S_OK)
    (void)logHRESULT (L"error calling GetStride() in uiImageAppend()", hr);

  for (int y = 0; y < pixelHeight; y++)
    {
      for (int x = 0; x < pixelWidth * 4; x += 4)
        {
          union
          {
            uint32_t v32;
            uint8_t  v8[4];
          } v;

          v.v32 = static_cast<uint32_t> (pix[x + 3]) << 24;
          v.v32 |= static_cast<uint32_t> (pix[x]) << 16;
          v.v32 |= static_cast<uint32_t> (pix[x + 1]) << 8;
          v.v32 |= static_cast<uint32_t> (pix[x + 2]);

          data[x]     = v.v8[0];
          data[x + 1] = v.v8[1];
          data[x + 2] = v.v8[2];
          data[x + 3] = v.v8[3];
        }
      pix += byteStride;
      data += realStride;
    }

  l->Release ();
  i->bitmaps->push_back (b);
}

struct matcher
{
  IWICBitmap *best;
  int         distX;
  int         distY;
  int         targetX;
  int         targetY;
  bool        foundLarger;
};

static void
match (IWICBitmap *b, matcher *m)
{
  UINT ux;
  UINT uy;

  const HRESULT hr = b->GetSize (&ux, &uy);
  if (hr != S_OK)
    (void)logHRESULT (L"error calling GetSize() in match()", hr);

  const int x = ux; // NOLINT(*-narrowing-conversions)
  const int y = uy; // NOLINT(*-narrowing-conversions)
  if (m->best == nullptr)
    goto writeMatch;

  if (x < m->targetX && y < m->targetY)
    if (m->foundLarger)
      return;

  if (x >= m->targetX && y >= m->targetY && !m->foundLarger)
    goto writeMatch;

  const int x2 = abs (m->targetX - x);
  const int y2 = abs (m->targetY - y);
  if (x2 < m->distX && y2 < m->distY)
    goto writeMatch;

  return;

writeMatch:
  if (x >= m->targetX && y >= m->targetY && !m->foundLarger)
    m->foundLarger = true;

  m->best  = b;
  m->distX = abs (m->targetX - x);
  m->distY = abs (m->targetY - y);
}

IWICBitmap *
uiprivImageAppropriateForDC (const uiImage *i, const HDC dc)
{
  matcher m;

  m.best        = nullptr;
  m.distX       = INT_MAX;
  m.distY       = INT_MAX;
  m.targetX     = MulDiv (i->width, GetDeviceCaps (dc, LOGPIXELSX), 96);  // NOLINT(*-narrowing-conversions)
  m.targetY     = MulDiv (i->height, GetDeviceCaps (dc, LOGPIXELSY), 96); // NOLINT(*-narrowing-conversions)
  m.foundLarger = false;

  for (IWICBitmap *b : *i->bitmaps)
    match (b, &m);
  return m.best;
}

HRESULT
uiprivWICToGDI (IWICBitmap *b, const HDC dc, int width, int height, HBITMAP *hb)
{
  UINT ux;
  UINT uy;

  IWICBitmapSource *src;

  BITMAPINFO bmi;
  BITMAP     bmp;
  void      *bits;

  HRESULT hr = b->GetSize (&ux, &uy);
  if (hr != S_OK)
    return hr;

  const int x = ux; // NOLINT(*-narrowing-conversions)
  const int y = uy; // NOLINT(*-narrowing-conversions)

  if (width == 0)
    width = x;

  if (height == 0)
    height = y;

  if (width == x && height == y)
    {
      b->AddRef (); // for the Release() later
      src = b;
    }
  else
    {
      IWICBitmapScaler    *scaler;
      WICPixelFormatGUID   guid;
      IWICFormatConverter *conv;

      hr = uiprivWICFactory->CreateBitmapScaler (&scaler);
      if (hr != S_OK)
        return hr;

      hr = scaler->Initialize (b, width, height, WICBitmapInterpolationModeNearestNeighbor);
      if (hr != S_OK)
        {
          scaler->Release ();
          return hr;
        }

      hr = scaler->GetPixelFormat (&guid);
      if (hr != S_OK)
        {
          scaler->Release ();
          return hr;
        }

      if (IsEqualGUID (guid, formatForGDI) != 0)
        {
          src = scaler;
        }

      else
        {
          hr = uiprivWICFactory->CreateFormatConverter (&conv);
          if (hr != S_OK)
            {
              scaler->Release ();
              return hr;
            }

          hr = conv->Initialize (scaler, formatForGDI, WICBitmapDitherTypeNone, nullptr, 0,
                                 WICBitmapPaletteTypeMedianCut);
          scaler->Release ();
          if (hr != S_OK)
            {
              conv->Release ();
              return hr;
            }
          src = conv;
        }
    }

  ZeroMemory (&bmi, sizeof (BITMAPINFO));
  bmi.bmiHeader.biSize        = sizeof (BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth       = width;
  bmi.bmiHeader.biHeight      = -height;
  bmi.bmiHeader.biPlanes      = 1;
  bmi.bmiHeader.biBitCount    = 32;
  bmi.bmiHeader.biCompression = BI_RGB;

  *hb = CreateDIBSection (dc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
  if (*hb == nullptr)
    {
      (void)logLastError (L"CreateDIBSection()");
      hr = E_FAIL;
      goto fail;
    }

  // now we need to figure out the stride of the image data GDI gave us
  if (GetObject (*hb, sizeof (BITMAP), &bmp) == 0)
    (void)logLastError (L"error calling GetObject() in uiprivWICToGDI()");
  hr = src->CopyPixels (nullptr, bmp.bmWidthBytes, bmp.bmWidthBytes * bmp.bmHeight, static_cast<BYTE *> (bits));

fail:
  if (*hb != nullptr && hr != S_OK)
    {
      DeleteObject (*hb);
      *hb = nullptr;
    }

  src->Release ();
  return hr;
}
