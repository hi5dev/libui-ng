#include "test.h"

#include <ui/button.h>
#include <ui/multiline_entry.h>

#include <stdio.h>

static void
meChanged (uiMultilineEntry *, void *data)
{
  (void)printf ("%s changed\n", (char *)data);
}

static void
setClicked (uiButton *, void *data)
{
  uiMultilineEntrySetText (uiMultilineEntry (data), "set");
}

static void
appendClicked (uiButton *, void *data)
{
  uiMultilineEntryAppend (uiMultilineEntry (data), "append\n");
}

static uiBox *half(uiMultilineEntry *(*mk)(void), const char *which)
{
  uiBox *vbox = newVerticalBox ();

  uiMultilineEntry *me = (*mk) ();
  uiMultilineEntryOnChanged (me, meChanged, (void *)which);
  uiBoxAppend (vbox, uiControl (me), 1);

  uiBox *hbox = newHorizontalBox ();
  uiBoxAppend (vbox, uiControl (hbox), 0);

  uiButton *button = uiNewButton ("Set");
  uiButtonOnClicked (button, setClicked, me);
  uiBoxAppend (hbox, uiControl (button), 0);

  button = uiNewButton ("Append");
  uiButtonOnClicked (button, appendClicked, me);
  uiBoxAppend (hbox, uiControl (button), 0);

  return vbox;
}

uiBox *
makePage12 (void)
{
  uiBox *page12 = newHorizontalBox ();

  uiBox *b = half (uiNewMultilineEntry, "wrap");
  uiBoxAppend (page12, uiControl (b), 1);

  b = half (uiNewNonWrappingMultilineEntry, "no wrap");
  uiBoxAppend (page12, uiControl (b), 1);

  return page12;
}
