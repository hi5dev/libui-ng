#include "attrstr.h"
#include "attribute_priv.h"
#include "debug.h"

#include <ui/attribute.h>
#include <ui/attributed_string.h>

#include <dwrite.h>

#include <cstdint>
#include <vector>

struct foreachParams
{
  const uint16_t                          *s;
  size_t                                   len;
  IDWriteTextLayout                       *layout;
  std::vector<drawTextBackgroundParams *> *backgroundParams;
};

static std::hash<double> doubleHash;

class combinedEffectsAttr final : public IUnknown
{
  ULONG        refcount;
  uiAttribute *colorAttr;
  uiAttribute *underlineAttr;
  uiAttribute *underlineColorAttr;

  void
  setAttribute (uiAttribute *a)
  {
    if (a == NULL)
      return;
    switch (uiAttributeGetType (a))
      {
      case uiAttributeTypeColor:
        if (this->colorAttr != NULL)
          uiprivAttributeRelease (this->colorAttr);
        this->colorAttr = uiprivAttributeRetain (a);
        break;
      case uiAttributeTypeUnderline:
        if (this->underlineAttr != NULL)
          uiprivAttributeRelease (this->underlineAttr);
        this->underlineAttr = uiprivAttributeRetain (a);
        break;
      case uiAttributeTypeUnderlineColor:
        if (this->underlineAttr != NULL)
          uiprivAttributeRelease (this->underlineAttr);
        this->underlineColorAttr = uiprivAttributeRetain (a);
        break;
      default:;
      }
  }

  static bool
  attrEqual (const uiAttribute *a, const uiAttribute *b)
  {
    if (a == NULL && b == NULL)

      return true;
    if (a == NULL || b == NULL)
      return false;

    return uiprivAttributeEqual (a, b) != 0;
  }

public:
  explicit
  combinedEffectsAttr (uiAttribute *a)
  {
    this->refcount           = 1;
    this->colorAttr          = NULL;
    this->underlineAttr      = NULL;
    this->underlineColorAttr = NULL;
    this->setAttribute (a);
  }

  virtual ~
  combinedEffectsAttr ()
  {
    if (this->colorAttr != NULL)
      uiprivAttributeRelease (this->colorAttr);
    if (this->underlineAttr != NULL)
      uiprivAttributeRelease (this->underlineAttr);
    if (this->underlineColorAttr != NULL)
      uiprivAttributeRelease (this->underlineColorAttr);
  }

  // IUnknown
  HRESULT STDMETHODCALLTYPE
  QueryInterface (REFIID riid, void **ppvObject) override
  {
    if (ppvObject == NULL)
      return E_POINTER;
    if (riid == IID_IUnknown)
      {
        this->AddRef ();
        *ppvObject = this;
        return S_OK;
      }
    *ppvObject = NULL;
    return E_NOINTERFACE;
  }

  ULONG STDMETHODCALLTYPE
  AddRef () override
  {
    this->refcount++;
    return this->refcount;
  }

  ULONG STDMETHODCALLTYPE
  Release () override
  {
    this->refcount--;
    if (this->refcount == 0)
      {
        delete this;
        return 0;
      }
    return this->refcount;
  }

  combinedEffectsAttr *
  cloneWith (uiAttribute *a) const
  {
    auto *const b = new combinedEffectsAttr (this->colorAttr);
    b->setAttribute (this->underlineAttr);
    b->setAttribute (this->underlineColorAttr);
    b->setAttribute (a);
    return b;
  }

  // and these are also needed by applyEffectsAttributes() below
  size_t
  hash () const noexcept
  {
    size_t ret = 0;
    double r;
    double g;
    double b;
    double a;

    uiUnderlineColor colorType;

    if (this->colorAttr != NULL)
      {
        uiAttributeColor (this->colorAttr, &r, &g, &b, &a);
        ret ^= doubleHash (r);
        ret ^= doubleHash (g);
        ret ^= doubleHash (b);
        ret ^= doubleHash (a);
      }
    if (this->underlineAttr != NULL)
      ret ^= static_cast<size_t> (uiAttributeUnderline (this->underlineAttr));
    if (this->underlineColorAttr != NULL)
      {
        uiAttributeUnderlineColor (this->underlineColorAttr, &colorType, &r, &g, &b, &a);
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
    if (b == NULL)
      return false;
    return attrEqual (this->colorAttr, b->colorAttr) && attrEqual (this->underlineAttr, b->underlineAttr)
           && attrEqual (this->underlineColorAttr, b->underlineColorAttr);
  }

  drawingEffectsAttr *
  toDrawingEffectsAttr (void)
  {
    drawingEffectsAttr *dea;
    double              r, g, b, a;
    uiUnderlineColor    colorType;

    dea = new drawingEffectsAttr;
    if (this->colorAttr != NULL)
      {
        uiAttributeColor (this->colorAttr, &r, &g, &b, &a);
        dea->setColor (r, g, b, a);
      }
    if (this->underlineAttr != NULL)
      dea->setUnderline (uiAttributeUnderline (this->underlineAttr));
    if (this->underlineColorAttr != NULL)
      {
        uiAttributeUnderlineColor (this->underlineColorAttr, &colorType, &r, &g, &b, &a);
        switch (colorType)
          {
          case uiUnderlineColorSpelling:
            r = 1.0;
            g = 0.0;
            b = 0.0;
            a = 1.0;
            break;
          case uiUnderlineColorGrammar:
            r = 0.0;
            g = 1.0;
            b = 0.0;
            a = 1.0;
            break;
          case uiUnderlineColorAuxiliary:
            r = 0.0;
            g = 0.0;
            b = 1.0;
            a = 1.0;
            break;
          default:
            break;
          }
        dea->setUnderlineColor (r, g, b, a);
      }
    return dea;
  }
};

// also needed by applyEffectsAttributes() below
// TODO provide all the fields of std::hash and std::equal_to?
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
  IUnknown            *u;
  combinedEffectsAttr *cea;
  DWRITE_TEXT_RANGE    range;
  size_t               diff;
  HRESULT              hr;

  while (start < end)
    {
      hr = p->layout->GetDrawingEffect (start, &u, &range);
      if (hr != S_OK)
        return hr;
      cea = (combinedEffectsAttr *)u;
      if (cea == NULL)
        cea = new combinedEffectsAttr (attr);
      else
        cea = cea->cloneWith (attr);
      // clamp range within [start, end)
      if (range.startPosition < start)
        {
          diff                = start - range.startPosition;
          range.startPosition = start;
          range.length -= diff;
        }
      if ((range.startPosition + range.length) > end)
        range.length = end - range.startPosition;
      hr = p->layout->SetDrawingEffect (cea, range);
      // SetDrawingEffect will AddRef(), so Release() our copy
      // (and we're abandoning early if that failed, so this will make sure things are cleaned up in that
case) 		cea->Release(); 		if (hr != S_OK) 			return hr; 		start += range.length;
	}
	return S_OK;
}

static void addBackgroundParams(const struct foreachParams *p, size_t start, size_t end, const uiAttribute *attr)
{
	struct drawTextBackgroundParams *params;

	params = uiprivNew(struct drawTextBackgroundParams);
	params->start = start;
	params->end = end;
	uiAttributeColor(attr, &(params->r), &(params->g), &(params->b), &(params->a));
	p->backgroundParams->push_back(params);
}

static uiForEach processAttribute(const uiAttributedString *s, const uiAttribute *attr, size_t start, size_t end,
void *data)
{
	struct foreachParams *p = (struct foreachParams *) data;
	DWRITE_TEXT_RANGE range;
	WCHAR *wfamily;
	BOOL hasUnderline;
	IDWriteTypography *dt;
	HRESULT hr;

	start = uiprivAttributedStringUTF8ToUTF16(s, start);
	end = uiprivAttributedStringUTF8ToUTF16(s, end);
	range.startPosition = start;
	range.length = end - start;
	switch (uiAttributeGetType(attr)) {
	case uiAttributeTypeFamily:
  wfamily = toUTF16 (uiAttributeFamily (attr));
  hr      = p->layout->SetFontFamilyName (wfamily, range);
  if (hr != S_OK)
    logHRESULT (L"error applying family name attribute", hr);
  uiprivFree (wfamily);
  break;
case uiAttributeTypeSize:
  hr = p->layout->SetFontSize (
// TODO unify with fontmatch.cpp and/or attrstr.hpp
#define pointSizeToDWriteSize(size) (size * (96.0 / 72.0))
      pointSizeToDWriteSize (uiAttributeSize (attr)), range);
      if (hr != S_OK)
        logHRESULT (L"error applying size attribute", hr);
      break;
    case uiAttributeTypeWeight:
      hr = p->layout->SetFontWeight (uiprivWeightToDWriteWeight (uiAttributeWeight (attr)), range);
      if (hr != S_OK)
        logHRESULT (L"error applying weight attribute", hr);
      break;
    case uiAttributeTypeItalic:
      hr = p->layout->SetFontStyle (uiprivItalicToDWriteStyle (uiAttributeItalic (attr)), range);
      if (hr != S_OK)
        logHRESULT (L"error applying italic attribute", hr);
      break;
    case uiAttributeTypeStretch:
      hr = p->layout->SetFontStretch (uiprivStretchToDWriteStretch (uiAttributeStretch (attr)), range);
      if (hr != S_OK)
        logHRESULT (L"error applying stretch attribute", hr);
      break;
    case uiAttributeTypeUnderline:
      // mark that we have an underline; otherwise, DirectWrite will never call our custom renderer's
      DrawUnderline () method hasUnderline = FALSE;
      if (uiAttributeUnderline (attr) != uiUnderlineNone)
        hasUnderline = TRUE;
      hr = p->layout->SetUnderline (hasUnderline, range);
      if (hr != S_OK)
        logHRESULT (L"error applying underline attribute", hr);
      // fall through to set the underline style through the drawing effect
      // fall through
    case uiAttributeTypeColor:
    case uiAttributeTypeUnderlineColor:
      // TODO const-correct this properly
      hr = addEffectAttributeToRange (p, start, end, (uiAttribute *)attr);
      if (hr != S_OK)
        logHRESULT (L"error applying effect (color, underline, or underline color) attribute", hr);
      break;
    case uiAttributeTypeBackground:
      addBackgroundParams (p, start, end, attr);
      break;
    case uiAttributeTypeFeatures:
      // only generate an attribute if not NULL
      // TODO do we still need to do this or not...
      if (uiAttributeFeatures (attr) == NULL)
        break;
      dt = uiprivOpenTypeFeaturesToIDWriteTypography (uiAttributeFeatures (attr));
      hr = p->layout->SetTypography (dt, range);
      if (hr != S_OK)
        logHRESULT (L"error applying features attribute", hr);
      dt->Release ();
      break;
    }
  return uiForEachContinue;
}

static HRESULT
applyEffectsAttributes (struct foreachParams *p)
{
  IUnknown            *u;
  combinedEffectsAttr *cea;
  drawingEffectsAttr  *dea;
  DWRITE_TEXT_RANGE    range;
  // here's the magic: this std::unordered_map will deduplicate all of our combinedEffectsAttrs, mapping all
  identical ones to a single drawingEffectsAttr
      // because drawingEffectsAttr is the *actual* drawing effect we want for rendering, we also replace the
      combinedEffectsAttrs with them in the IDWriteTextLayout at the same time
          // note the use of our custom hash and equal_to implementations
              std::unordered_map<combinedEffectsAttr *, drawingEffectsAttr *, applyEffectsHash, applyEffectsEqualTo>
              effects;
  HRESULT     hr;

  // go through, replacing every combinedEffectsAttr with the proper drawingEffectsAttr
  range.startPosition = 0;
  // and in case this while loop never runs, make hr valid to start with
  hr = S_OK;
  while (range.startPosition < p->len)
    {
      hr = p->layout->GetDrawingEffect (range.startPosition, &u, &range);
      if (hr != S_OK)
        // note that we are breaking instead of returning; this allows us to clean up on failure
        break;
      cea = (combinedEffectsAttr *)u;
      if (cea != NULL)
        {
          auto diter = effects.find (cea);
          if (diter != effects.end ())
            dea = diter->second;
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
  for (auto iter = effects.begin (); iter != effects.end (); iter++)
    {
      iter->first->Release ();
      iter->second->Release ();
    }
  return hr;
}

void
uiprivAttributedStringApplyAttributesToDWriteTextLayout (
    uiDrawTextLayoutParams *p, IDWriteTextLayout *layout,
    std::vector<struct drawTextBackgroundParams *> **backgroundParams)
{
  struct foreachParams fep;
  HRESULT              hr;

  fep.s                = uiprivAttributedStringUTF16String (p->String);
  fep.len              = uiprivAttributedStringUTF16Len (p->String);
  fep.layout           = layout;
  fep.backgroundParams = new std::vector<struct drawTextBackgroundParams *>;
  uiAttributedStringForEachAttribute (p->String, processAttribute, &fep);
  hr = applyEffectsAttributes (&fep);
  if (hr != S_OK)
    (void)logHRESULT (L"error applying effects attributes", hr);
  *backgroundParams = fep.backgroundParams;
}
