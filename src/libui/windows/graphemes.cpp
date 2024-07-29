#include <windows.h>

#include <attrstr.h>
#include <uipriv.h>

int
uiprivGraphemesTakesUTF16 ()
{
  return 1;
}

uiprivGraphemes *
uiprivNewGraphemes (void *s, const size_t len)
{
  auto *g = uiprivNew (uiprivGraphemes);
  g->len  = 0;

  auto *str = static_cast<WCHAR *> (s);
  while (*str != L'\0')
    {
      g->len++;
      str = CharNextW (str);
    }

  g->pointsToGraphemes = static_cast<size_t *> (uiprivAlloc ((len + 1) * sizeof (size_t), "size_t[] (graphemes)"));
  g->graphemesToPoints = static_cast<size_t *> (uiprivAlloc ((g->len + 1) * sizeof (size_t), "size_t[] (graphemes)"));

  size_t *pPTG = g->pointsToGraphemes;
  size_t *pGTP = g->graphemesToPoints;

  str = static_cast<WCHAR *> (s);
  while (*str != L'\0')
    {
      ptrdiff_t nextoff = 0;

      if (IS_HIGH_SURROGATE (*str))
        nextoff = 1;

      WCHAR *next = CharNextW (str + nextoff);
      if (IS_LOW_SURROGATE (*next))
        next--;

      *pGTP = pPTG - g->pointsToGraphemes;
      for (WCHAR *p = str; p < next; p++)
        *pPTG++ = pGTP - g->graphemesToPoints;

      pGTP++;

      str = next;
    }

  *pGTP = pPTG - g->pointsToGraphemes;
  *pPTG = pGTP - g->graphemesToPoints;

  return g;
}
