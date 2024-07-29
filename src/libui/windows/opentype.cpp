#include "opentype.h"
#include "debug.h"
#include "dwrite_impl.h"

#include <ui/api.h>

#include <cstdint>

static uiForEach
addToTypography (const uiOpenTypeFeatures *, const char a, const char b, const char c, const char d,
                 const uint32_t value, void *data)
{
  auto *dt = static_cast<IDWriteTypography *> (data);

  DWRITE_FONT_FEATURE dff;
  ZeroMemory (&dff, sizeof (DWRITE_FONT_FEATURE));

  dff.nameTag   = static_cast<DWRITE_FONT_FEATURE_TAG> (DWRITE_MAKE_OPENTYPE_TAG (a, b, c, d));
  dff.parameter = value;

  const HRESULT hr = dt->AddFontFeature (dff);
  if (hr != S_OK)
    (void)logHRESULT (L"error adding OpenType feature to IDWriteTypography", hr);

  return uiForEachContinue;
}

IDWriteTypography *
uiprivOpenTypeFeaturesToIDWriteTypography (const uiOpenTypeFeatures *otf)
{
  IDWriteTypography *dt;

  const HRESULT hr = dwfactory->CreateTypography (&dt);
  if (hr != S_OK)
    (void)logHRESULT (L"error creating IDWriteTypography", hr);

  uiOpenTypeFeaturesForEach (otf, addToTypography, dt);

  return dt;
}
