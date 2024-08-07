// 22 april 2015
#include "test.h"

#include <stdlib.h>

struct thing
{
  void *ptr;
  int   type;
};

static struct thing *things = NULL;
static size_t        len    = 0;
static size_t        cap    = 0;

#define grow 32

static void *
append (void *thing, const int type)
{
  if (len >= cap)
    {
      cap += grow;
      things = (struct thing *)realloc (things, cap * sizeof (struct thing));
      if (things == NULL)
        die ("reallocating things array in test/spaced.c append()");
    }
  things[len].ptr  = thing;
  things[len].type = type;
  len++;
  return things[len - 1].ptr;
}

enum types
{
  window,
  box,
  tab,
  group,
  form,
  grid,
};

void
setSpaced (const int spaced)
{
  size_t j, n;

  for (size_t i = 0; i < len; i++)
    {
      void *p = things[i].ptr;
      switch (things[i].type)
        {
        case window:
          uiWindowSetMargined (uiWindow (p), spaced);
          break;

        case box:
          uiBoxSetPadded (uiBox (p), spaced);
          break;

        case tab:
          n = uiTabNumPages (uiTab (p));
          for (j = 0; j < n; j++)
            uiTabSetMargined (uiTab (p), j, spaced);
          break;

        case group:
          uiGroupSetMargined (uiGroup (p), spaced);
          break;

        case form:
          uiFormSetPadded (uiForm (p), spaced);
          break;

        case grid:
          uiGridSetPadded (uiGrid (p), spaced);
          break;

        default:
          break;
        }
    }
}

void
querySpaced (char out[12]) // more than enough
{
  int    m = 0;
  int    p = 0;
  size_t j, n;

  for (size_t i = 0; i < len; i++)
    {
      void *pp = things[i].ptr;
      switch (things[i].type)
        {
        case window:
          if (uiWindowMargined (uiWindow (pp)))
            m++;
          break;

        case box:
          p = uiBoxPadded (uiBox (pp));
          break;

        case tab:
          n = uiTabNumPages (uiTab (pp));
          for (j = 0; j < n; j++)
            if (uiTabMargined (uiTab (pp), j))
              m++;
          break;

        case group:
          if (uiGroupMargined (uiGroup (pp)))
            m++;
          break;

        default:;
          break;
        }
    }

  out[0] = 'm';
  out[1] = ' ';
  out[2] = '0' + m;
  out[3] = ' ';
  out[4] = 'p';
  out[5] = ' ';
  out[6] = '0';
  if (p)
    out[6] = '1';
  out[7] = '\0';
}

uiWindow *
newWindow (const char *title, const int width, const int height, const int hasMenubar)
{
  uiWindow *w = uiNewWindow (title, width, height, hasMenubar);
  append (w, window);
  return w;
}

uiBox *
newHorizontalBox (void)
{
  uiBox *b = (*newhbox) ();
  append (b, box);
  return b;
}

uiBox *
newVerticalBox (void)
{
  uiBox *b = (*newvbox) ();
  append (b, box);
  return b;
}

uiTab *
newTab (void)
{
  uiTab *t = uiNewTab ();
  append (t, tab);
  return t;
}

uiGroup *
newGroup (const char *text)
{
  uiGroup *g = uiNewGroup (text);
  append (g, group);
  return g;
}

uiForm *
newForm (void)
{
  uiForm *f = uiNewForm ();
  append (f, form);
  return f;
}

uiGrid *
newGrid (void)
{
  uiGrid *g = uiNewGrid ();
  append (g, grid);
  return g;
}
