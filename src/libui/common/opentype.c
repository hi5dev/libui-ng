#include "attrstr.h"
#include "ui.h"
#include "uipriv.h"

#include <stdlib.h>

struct feature
{
  char     a;
  char     b;
  char     c;
  char     d;
  uint32_t value;
};

struct uiOpenTypeFeatures
{
  struct feature *data;
  size_t          len;
  size_t          cap;
};

#define bytecount(n) ((n) * sizeof (struct feature))

uiOpenTypeFeatures *
uiNewOpenTypeFeatures (void)
{
  uiOpenTypeFeatures *otf = uiprivNew (uiOpenTypeFeatures);
  otf->cap                = 16;
  otf->data               = (struct feature *)uiprivAlloc (bytecount (otf->cap), "struct feature[]");
  otf->len                = 0;
  return otf;
}

void
uiFreeOpenTypeFeatures (uiOpenTypeFeatures *otf)
{
  uiprivFree (otf->data);
  uiprivFree (otf);
}

uiOpenTypeFeatures *
uiOpenTypeFeaturesClone (const uiOpenTypeFeatures *otf)
{
  uiOpenTypeFeatures *ret = uiprivNew (uiOpenTypeFeatures);
  ret->len                = otf->len;
  ret->cap                = otf->cap;
  ret->data               = (struct feature *)uiprivAlloc (bytecount (ret->cap), "struct feature[]");
  memset (ret->data, 0, bytecount (ret->cap));
  memmove (ret->data, otf->data, bytecount (ret->len));
  return ret;
}

#define intdiff(a, b) (((int)(a)) - ((int)(b)))

static int
featurecmp (const void *a, const void *b)
{
  const struct feature *f = a;
  const struct feature *g = b;

  if (f->a != g->a)
    return intdiff (f->a, g->a);

  if (f->b != g->b)
    return intdiff (f->b, g->b);

  if (f->c != g->c)
    return intdiff (f->c, g->c);

  return intdiff (f->d, g->d);
}

static struct feature
mkkey (const char a, const char b, const char c, const char d)
{
  struct feature f;

  f.a = a;
  f.b = b;
  f.c = c;
  f.d = d;
  return f;
}

#define find(pkey, otf) bsearch (pkey, otf->data, otf->len, sizeof (struct feature), featurecmp)

void
uiOpenTypeFeaturesAdd (uiOpenTypeFeatures *otf, const char a, const char b, const char c, const char d,
                       const uint32_t value)
{
  struct feature key;

  // replace existing value if any
  key               = mkkey (a, b, c, d);
  struct feature *f = find (&key, otf);
  if (f != NULL)
    {
      f->value = value;
      return;
    }

  // if we got here, the tag is new
  if (otf->len == otf->cap)
    {
      otf->cap *= 2;
      otf->data = (struct feature *)uiprivRealloc (otf->data, bytecount (otf->cap), "struct feature[]");
    }
  f        = otf->data + otf->len;
  f->a     = a;
  f->b     = b;
  f->c     = c;
  f->d     = d;
  f->value = value;

  otf->len++;
  qsort (otf->data, otf->len, sizeof (struct feature), featurecmp);
}

void
uiOpenTypeFeaturesRemove (uiOpenTypeFeatures *otf, const char a, const char b, const char c, const char d)
{
  struct feature key;

  key               = mkkey (a, b, c, d);
  struct feature *f = find (&key, otf);
  if (f == NULL)
    return;

  const ptrdiff_t index = f - otf->data;
  const size_t    count = otf->len - index - 1;
  memmove (f + 1, f, bytecount (count));
  otf->len--;
}

int
uiOpenTypeFeaturesGet (const uiOpenTypeFeatures *otf, const char a, const char b, const char c, const char d,
                       uint32_t *value)
{
  struct feature key;

  key                     = mkkey (a, b, c, d);
  const struct feature *f = find (&key, otf);
  if (f == NULL)
    return 0;
  *value = f->value;
  return 1;
}

void
uiOpenTypeFeaturesForEach (const uiOpenTypeFeatures *otf, const uiOpenTypeFeaturesForEachFunc f, void *data)
{

  const struct feature *p = otf->data;
  for (size_t n = 0; n < otf->len; n++)
    {
      const uiForEach ret = (*f) (otf, p->a, p->b, p->c, p->d, p->value, data);

      if (ret == uiForEachStop)
        return;

      p++;
    }
}

int
uiprivOpenTypeFeaturesEqual (const uiOpenTypeFeatures *a, const uiOpenTypeFeatures *b)
{
  if (a == b)
    return 1;

  if (a->len != b->len)
    return 0;

  return memcmp (a->data, b->data, bytecount (a->len)) == 0;
}
