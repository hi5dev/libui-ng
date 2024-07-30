#include <dwrite.h>

#include "attribute_priv.h"
#include "attrstr.h"
#include "attrstr_win32.h"
#include "debug.h"
#include "fontmatch.h"
#include "opentype.h"
#include "utf16.h"

#include <ui/attribute.h>
#include <ui/attributed_string.h>
#include <ui/draw.h>

#include <cstdint>
#include <uipriv.h>
#include <unordered_map>
#include <vector>

#define pointSizeToDWriteSize(size) (size * (96.0 / 72.0))

static std::hash<double> doubleHash;

typedef struct foreachParams
{
  const uint16_t *s;

  size_t len;

  IDWriteTextLayout *layout;

  std::vector<drawTextBackgroundParams *> *backgroundParams;
} foreachParams;

class combinedEffectsAttr final : public IUnknown
{
  ULONG refcount;

  uiAttribute *colorAttr;

  uiAttribute *underlineAttr;

  uiAttribute *underlineColorAttr;

  void
  setAttribute (uiAttribute *a)
  {
    if (a == nullptr)
      return;

    switch (uiAttributeGetType (a))
      {
      case uiAttributeTypeColor:
        {
          if (colorAttr != nullptr)
            uiprivAttributeRelease (colorAttr);

          colorAttr = uiprivAttributeRetain (a);

          break;
        }
      case uiAttributeTypeUnderline:
        {
          if (underlineAttr != nullptr)
            uiprivAttributeRelease (underlineAttr);

          underlineAttr = uiprivAttributeRetain (a);

          break;
        }
      case uiAttributeTypeUnderlineColor:
        {
          if (underlineAttr != nullptr)
            uiprivAttributeRelease (underlineAttr);

          underlineColorAttr = uiprivAttributeRetain (a);

          break;
        }

      default:
        break;
      }
  }

  static bool
  attrEqual (const uiAttribute *a, const uiAttribute *b)
  {
    if (a == nullptr && b == nullptr)
      return true;

    if (a == nullptr || b == nullptr)
      return false;

    return uiprivAttributeEqual (a, b) != 0;
  }

public:
  explicit
  combinedEffectsAttr (uiAttribute *a)
  {
    this->refcount           = 1;
    this->colorAttr          = nullptr;
    this->underlineAttr      = nullptr;
    this->underlineColorAttr = nullptr;
    this->setAttribute (a);
  }

  virtual ~
  combinedEffectsAttr ()
  {
    if (colorAttr != nullptr)
      uiprivAttributeRelease (colorAttr);

    if (underlineAttr != nullptr)
      uiprivAttributeRelease (underlineAttr);

    if (underlineColorAttr != nullptr)
      uiprivAttributeRelease (underlineColorAttr);
  }

  HRESULT STDMETHODCALLTYPE
  QueryInterface (REFIID riid, void **ppvObject) override
  {
    if (ppvObject == nullptr)
      return E_POINTER;

    if (riid == IID_IUnknown)
      {
        AddRef ();
        *ppvObject = this;
        return S_OK;
      }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
  }

  ULONG STDMETHODCALLTYPE
  AddRef () override
  {
    this->refcount++;

    return refcount;
  }

  ULONG STDMETHODCALLTYPE
  Release () override
  {
    this->refcount--;

    if (refcount == 0)
      {
        delete this;
        return 0;
      }

    return refcount;
  }

  combinedEffectsAttr *
  cloneWith (uiAttribute *a) const
  {
    auto *const b = new combinedEffectsAttr (colorAttr);

    b->setAttribute (underlineAttr);
    b->setAttribute (underlineColorAttr);
    b->setAttribute (a);

    return b;
  }

  size_t
  hash () const noexcept
  {
    size_t ret = 0;

    double r;
    double g;
    double b;
    double a;

    uiUnderlineColor colorType;

    if (colorAttr != nullptr)
      {
        uiAttributeColor (colorAttr, &r, &g, &b, &a);
        ret ^= doubleHash (r);
        ret ^= doubleHash (g);
        ret ^= doubleHash (b);
        ret ^= doubleHash (a);
      }

    if (underlineAttr != nullptr)
      ret ^= static_cast<size_t> (uiAttributeUnderline (underlineAttr));

    if (underlineColorAttr != nullptr)
      {
        uiAttributeUnderlineColor (underlineColorAttr, &colorType, &r, &g, &b, &a);
        ret ^= static_cast<size_t> (colorType);
        ret ^= doubleHash (r);
        ret ^= doubleHash (g);
        ret ^= doubleHash (b);
        ret ^= doubleHash (a);
      }

    return ret;
  }

  bool
  equals (const combinedEffectsAttr *b) const
  {
    if (b == nullptr)
      return false;

    return attrEqual (colorAttr, b->colorAttr) && attrEqual (underlineAttr, b->underlineAttr)
           && attrEqual (underlineColorAttr, b->underlineColorAttr);
  }

  drawingEffectsAttr *
  toDrawingEffectsAttr () const
  {
    double r;
    double g;
    double b;
    double a;

    uiUnderlineColor colorType;

    auto *dea = new drawingEffectsAttr;
    if (colorAttr != nullptr)
      {
        uiAttributeColor (colorAttr, &r, &g, &b, &a);
        dea->setColor (r, g, b, a);
      }

    if (underlineAttr != nullptr)
      dea->setUnderline (uiAttributeUnderline (underlineAttr));

    if (underlineColorAttr != nullptr)
      {
        uiAttributeUnderlineColor (underlineColorAttr, &colorType, &r, &g, &b, &a);
        switch (colorType)
          {
          case uiUnderlineColorSpelling:
            {
              r = 1.0;
              g = 0.0;
              b = 0.0;
              a = 1.0;
              break;
            }

          case uiUnderlineColorGrammar:
            {
              r = 0.0;
              g = 1.0;
              b = 0.0;
              a = 1.0;
              break;
            }

          case uiUnderlineColorAuxiliary:
            {
              r = 0.0;
              g = 0.0;
              b = 1.0;
              a = 1.0;
              break;
            }

          default:
            break;
          }

        dea->setUnderlineColor (r, g, b, a);
      }

    return dea;
  }
};

class applyEffectsHash
{
public:
  typedef combinedEffectsAttr *ceaptr;

  size_t

  operator() (ceaptr const &cea) const noexcept
  {
    return cea->hash ();
  }
};

class applyEffectsEqualTo
{
public:
  typedef combinedEffectsAttr *ceaptr;

  bool
  operator() (const ceaptr &a, const ceaptr &b) const
  {
    return a->equals (b);
  }
};

static HRESULT
addEffectAttributeToRange (const foreachParams *p, size_t start, const size_t end, uiAttribute *attr)
{
  IUnknown *u;

  DWRITE_TEXT_RANGE range;

  while (start < end)
    {
      HRESULT hr = p->layout->GetDrawingEffect (start, &u, &range);
      if (hr != S_OK)
        return hr;

      auto *cea = static_cast<combinedEffectsAttr *> (u);
      if (cea == nullptr)
        cea = new combinedEffectsAttr (attr);

      else
        cea = cea->cloneWith (attr);

      // clamp range within [start, end)
      if (range.startPosition < start)
        {
          const size_t diff   = start - range.startPosition;
          range.startPosition = start;
          range.length -= diff;
        }
      if (range.startPosition + range.length > end)
        range.length = end - range.startPosition;

      hr = p->layout->SetDrawingEffect (cea, range);

      cea->Release ();
      if (hr != S_OK)
        return hr;

      start += range.length;
    }

  return S_OK;
}

static void
addBackgroundParams (const foreachParams *p, const size_t start, const size_t end, const uiAttribute *attr)
{

  auto *params  = uiprivNew (struct drawTextBackgroundParams);
  params->start = start;
  params->end   = end;

  uiAttributeColor (attr, &params->r, &params->g, &params->b, &params->a);

  p->backgroundParams->push_back (params);
}

static uiForEach
processAttribute (const uiAttributedString *s, const uiAttribute *attr, size_t start, size_t end, void *data)
{
  const auto *const p = static_cast<struct foreachParams *> (data);

  DWRITE_TEXT_RANGE range;

  HRESULT hr;

  start = uiprivAttributedStringUTF8ToUTF16 (s, start);
  end   = uiprivAttributedStringUTF8ToUTF16 (s, end);

  range.startPosition = start;
  range.length        = end - start;

  switch (uiAttributeGetType (attr))
    {
    case uiAttributeTypeFamily:
      {
        WCHAR *wfamily = toUTF16 (uiAttributeFamily (attr));

        hr = p->layout->SetFontFamilyName (wfamily, range);
        if (hr != S_OK)
          (void)logHRESULT (L"error applying family name attribute", hr);

        uiprivFree (wfamily);

        break;
      }

    case uiAttributeTypeSize:
      {
        hr = p->layout->SetFontSize (pointSizeToDWriteSize (uiAttributeSize (attr)), range);

        if (hr != S_OK)
          (void)logHRESULT (L"error applying size attribute", hr);

        break;
      }

    case uiAttributeTypeWeight:
      {
        hr = p->layout->SetFontWeight (uiprivWeightToDWriteWeight (uiAttributeWeight (attr)), range);

        if (hr != S_OK)
          (void)logHRESULT (L"error applying weight attribute", hr);

        break;
      }

    case uiAttributeTypeItalic:
      {
        hr = p->layout->SetFontStyle (uiprivItalicToDWriteStyle (uiAttributeItalic (attr)), range);

        if (hr != S_OK)
          (void)logHRESULT (L"error applying italic attribute", hr);

        break;
      }

    case uiAttributeTypeStretch:
      {
        hr = p->layout->SetFontStretch (uiprivStretchToDWriteStretch (uiAttributeStretch (attr)), range);

        if (hr != S_OK)
          (void)logHRESULT (L"error applying stretch attribute", hr);

        break;
      }

    case uiAttributeTypeUnderline:
      {
        BOOL hasUnderline = FALSE;

        if (uiAttributeUnderline (attr) != uiUnderlineNone)
          hasUnderline = TRUE;

        hr = p->layout->SetUnderline (hasUnderline, range);

        if (hr != S_OK)
          (void)logHRESULT (L"error applying underline attribute", hr);

        [[fallthrough]];
      }

    case uiAttributeTypeColor:
      {
        [[fallthrough]];
      }

    case uiAttributeTypeUnderlineColor:
      {
        hr = addEffectAttributeToRange (p, start, end, const_cast<uiAttribute *> (attr));

        if (hr != S_OK)
          (void)logHRESULT (L"error applying effect (color, underline, or underline color) attribute", hr);

        break;
      }

    case uiAttributeTypeBackground:
      {
        addBackgroundParams (p, start, end, attr);
        break;
      }

    case uiAttributeTypeFeatures:
      {
        // only generate an attribute if not NULL
        if (uiAttributeFeatures (attr) == nullptr)
          break;

        IDWriteTypography *dt = uiprivOpenTypeFeaturesToIDWriteTypography (uiAttributeFeatures (attr));

        hr = p->layout->SetTypography (dt, range);
        if (hr != S_OK)
          (void)logHRESULT (L"error applying features attribute", hr);

        dt->Release ();

        break;
      }

    default:
      break;
    }

  return uiForEachContinue;
}

static HRESULT
applyEffectsAttributes (const foreachParams *p)
{
  IUnknown *u;

  drawingEffectsAttr *dea;

  DWRITE_TEXT_RANGE range;

  std::unordered_map<combinedEffectsAttr *, drawingEffectsAttr *, applyEffectsHash, applyEffectsEqualTo> effects;

  // go through, replacing every combinedEffectsAttr with the proper drawingEffectsAttr
  range.startPosition = 0;

  // and in case this while loop never runs, make hr valid to start with
  HRESULT hr = S_OK;

  while (range.startPosition < p->len)
    {
      hr = p->layout->GetDrawingEffect (range.startPosition, &u, &range);
      if (hr != S_OK)
        break;

      auto *cea = static_cast<combinedEffectsAttr *> (u);

      if (cea != nullptr)
        {
          auto diter = effects.find (cea);

          if (diter != effects.end ())
            {
              dea = diter->second;
            }

          else
            {
              dea = cea->toDrawingEffectsAttr ();
              effects.insert ({ cea, dea });
            }

          hr = p->layout->SetDrawingEffect (dea, range);

          // don't release dea; we need the reference that's inside the map
          // (we don't take extra references on lookup, so this will be fine)
          if (hr != S_OK)
            break;
        }

      range.startPosition += range.length;
    }

  // and clean up, finally destroying the combinedEffectAttrs too
  // we do this in the case of failure as well, to make sure everything is properly cleaned up
  for (auto iter = effects.begin (); iter != effects.end (); ++iter)
    {
      iter->first->Release ();
      iter->second->Release ();
    }

  return hr;
}

void
uiprivAttributedStringApplyAttributesToDWriteTextLayout (uiDrawTextLayoutParams *p, IDWriteTextLayout *layout,
                                                         std::vector<drawTextBackgroundParams *> **backgroundFuncs)
{
  foreachParams fep;
  fep.s                = uiprivAttributedStringUTF16String (p->String);
  fep.len              = uiprivAttributedStringUTF16Len (p->String);
  fep.layout           = layout;
  fep.backgroundParams = new std::vector<drawTextBackgroundParams *>;

  uiAttributedStringForEachAttribute (p->String, processAttribute, &fep);

  const HRESULT hr = applyEffectsAttributes (&fep);
  if (hr != S_OK)
    (void)logHRESULT (L"error applying effects attributes", hr);

  *backgroundFuncs = fep.backgroundParams;
}

drawingEffectsAttr::
drawingEffectsAttr ()
    : refcount (0), hasColor (false), r (0), g (0), b (0), a (0), hasUnderline (false), u (),
      hasUnderlineColor (false), ur (0), ug (0), ub (0), ua (0)
{
}
