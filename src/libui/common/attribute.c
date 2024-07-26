#include "attribute_priv.h"

#include "attrstr.h"
#include "uipriv.h"
#include "userbugs.h"

#include <string.h>

static uiAttribute *
newAttribute (const uiAttributeType type)
{
  uiAttribute *a = uiprivNew (uiAttribute);
  a->ownedByUser = 1;
  a->refcount    = 0;
  a->type        = type;
  return a;
}

static void
destroy (uiAttribute *a)
{
  switch (a->type)
    {
    case uiAttributeTypeFamily:
      uiprivFree (a->u.family);
      break;

    case uiAttributeTypeFeatures:
      uiFreeOpenTypeFeatures (a->u.features);
      break;

    default:
      break;
    }

  uiprivFree (a);
}

uiAttribute *
uiprivAttributeRetain (uiAttribute *a)
{
  a->ownedByUser = 0;
  a->refcount++;
  return a;
}

void
uiprivAttributeRelease (uiAttribute *a)
{
  if (a->ownedByUser)
    uiprivImplBug ("Can't release attribute we don't own %p", a);

  a->refcount--;
  if (a->refcount == 0)
    destroy (a);
}

void
uiFreeAttribute (uiAttribute *a)
{
  if (!a->ownedByUser)
    uiprivImplBug ("Can't release attribute we don't own %p", a);

  destroy (a);
}

uiAttributeType
uiAttributeGetType (const uiAttribute *a)
{
  return a->type;
}

uiAttribute *
uiNewFamilyAttribute (const char *family)
{
  uiAttribute *a = newAttribute (uiAttributeTypeFamily);
  a->u.family    = (char *)uiprivAlloc ((strlen (family) + 1) * sizeof (char), "char[] (uiAttribute)");
  strcpy (a->u.family, family);
  return a;
}

const char *
uiAttributeFamily (const uiAttribute *a)
{
  return a->u.family;
}

uiAttribute *
uiNewSizeAttribute (const double size)
{
  uiAttribute *a = newAttribute (uiAttributeTypeSize);
  a->u.size      = size;
  return a;
}

double
uiAttributeSize (const uiAttribute *a)
{
  return a->u.size;
}

uiAttribute *
uiNewWeightAttribute (const uiTextWeight weight)
{
  uiAttribute *a = newAttribute (uiAttributeTypeWeight);
  a->u.weight    = weight;
  return a;
}

uiTextWeight
uiAttributeWeight (const uiAttribute *a)
{
  return a->u.weight;
}

uiAttribute *
uiNewItalicAttribute (const uiTextItalic italic)
{
  uiAttribute *a = newAttribute (uiAttributeTypeItalic);
  a->u.italic    = italic;
  return a;
}

uiTextItalic
uiAttributeItalic (const uiAttribute *a)
{
  return a->u.italic;
}

uiAttribute *
uiNewStretchAttribute (const uiTextStretch stretch)
{
  uiAttribute *a = newAttribute (uiAttributeTypeStretch);
  a->u.stretch   = stretch;
  return a;
}

uiTextStretch
uiAttributeStretch (const uiAttribute *a)
{
  return a->u.stretch;
}

uiAttribute *
uiNewColorAttribute (const double r, const double g, const double b, const double a)
{
  uiAttribute *at = newAttribute (uiAttributeTypeColor);
  at->u.color.r   = r;
  at->u.color.g   = g;
  at->u.color.b   = b;
  at->u.color.a   = a;
  return at;
}

void
uiAttributeColor (const uiAttribute *a, double *r, double *g, double *b, double *alpha)
{
  *r     = a->u.color.r;
  *g     = a->u.color.g;
  *b     = a->u.color.b;
  *alpha = a->u.color.a;
}

uiAttribute *
uiNewBackgroundAttribute (const double r, const double g, const double b, const double a)
{
  uiAttribute *at = newAttribute (uiAttributeTypeBackground);
  at->u.color.r   = r;
  at->u.color.g   = g;
  at->u.color.b   = b;
  at->u.color.a   = a;
  return at;
}

uiAttribute *
uiNewUnderlineAttribute (const uiUnderline u)
{
  uiAttribute *a = newAttribute (uiAttributeTypeUnderline);
  a->u.underline = u;
  return a;
}

uiUnderline
uiAttributeUnderline (const uiAttribute *a)
{
  return a->u.underline;
}

uiAttribute *
uiNewUnderlineColorAttribute (const uiUnderlineColor u, const double r, const double g, const double b, const double a)
{
  uiAttribute *at            = uiNewColorAttribute (r, g, b, a);
  at->type                   = uiAttributeTypeUnderlineColor;
  at->u.color.underlineColor = u;
  return at;
}

void
uiAttributeUnderlineColor (const uiAttribute *a, uiUnderlineColor *u, double *r, double *g, double *b, double *alpha)
{
  *u = a->u.color.underlineColor;
  uiAttributeColor (a, r, g, b, alpha);
}

uiAttribute *
uiNewFeaturesAttribute (const uiOpenTypeFeatures *otf)
{
  uiAttribute *a = newAttribute (uiAttributeTypeFeatures);
  a->u.features  = uiOpenTypeFeaturesClone (otf);
  return a;
}

const uiOpenTypeFeatures *
uiAttributeFeatures (const uiAttribute *a)
{
  return a->u.features;
}

int
uiprivAttributeEqual (const uiAttribute *a, const uiAttribute *b)
{
  if (a == b)
    return 1;

  if (a->type != b->type)
    return 0;

  switch (a->type)
    {
    case uiAttributeTypeFamily:
      return uiprivStricmp (a->u.family, b->u.family);

    case uiAttributeTypeSize:
      return a->u.size == b->u.size;

    case uiAttributeTypeWeight:
      return a->u.weight == b->u.weight;

    case uiAttributeTypeItalic:
      return a->u.italic == b->u.italic;

    case uiAttributeTypeStretch:
      return a->u.stretch == b->u.stretch;

    case uiAttributeTypeUnderline:
      return a->u.underline == b->u.underline;

    case uiAttributeTypeUnderlineColor:
      if (a->u.color.underlineColor != b->u.color.underlineColor)
        return 0;
      // fallthrough

    case uiAttributeTypeColor:
      // fallthrough

    case uiAttributeTypeBackground:
      return a->u.color.r == b->u.color.r && a->u.color.g == b->u.color.g && a->u.color.b == b->u.color.b
             && a->u.color.a == b->u.color.a;

    case uiAttributeTypeFeatures:
      return uiprivOpenTypeFeaturesEqual (a->u.features, b->u.features);

    default:
      break;
    }

  return 0;
}
