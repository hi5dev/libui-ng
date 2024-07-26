#include "test.h"

#include <ui/box.h>
#include <ui/group.h>
#include <ui/tab.h>

static uiGroup *
newg (const char *n, const int s)
{
  uiGroup *g = uiNewGroup (n);

  if (s)
    uiGroupSetChild (g, NULL);

  return g;
}

static uiTab *
newt (const int tt)
{
  uiTab *t = uiNewTab ();

  if (tt)
    uiTabAppend (t, "Test", NULL);

  return t;
}

uiBox *makePage11(void)
{
  uiBox *page11 = newHorizontalBox ();

  uiBox *ns = newVerticalBox ();
  uiBoxAppend (ns, uiControl (newg ("", 0)), 0);
  uiBoxAppend (ns, uiControl (newg ("", 1)), 0);
  uiBoxAppend (ns, uiControl (newg ("Group", 0)), 0);
  uiBoxAppend (ns, uiControl (newg ("Group", 1)), 0);
  uiBoxAppend (ns, uiControl (newt (0)), 0);
  uiBoxAppend (ns, uiControl (newt (1)), 0);
  uiBoxAppend (page11, uiControl (ns), 1);

  uiBox *s = newVerticalBox ();
  uiBoxAppend (s, uiControl (newg ("", 0)), 1);
  uiBoxAppend (s, uiControl (newg ("", 1)), 1);
  uiBoxAppend (s, uiControl (newg ("Group", 0)), 1);
  uiBoxAppend (s, uiControl (newg ("Group", 1)), 1);
  uiBoxAppend (s, uiControl (newt (0)), 1);
  uiBoxAppend (s, uiControl (newt (1)), 1);
  uiBoxAppend (page11, uiControl (s), 1);

  return page11;
}
