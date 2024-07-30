#include "qa.h"

#include <ui/box.h>
#include <ui/multiline_entry.h>

uiControl *
qaMakeGuide (uiControl *c, const char *text)
{
  uiBox *hbox = uiNewHorizontalBox ();
  uiBoxSetPadded (hbox, 1);

  uiBox *vbox = uiNewVerticalBox ();
  uiBoxAppend (vbox, c, 0);
  uiBoxAppend (hbox, uiControl (vbox), 1);

  uiMultilineEntry *guide = uiNewMultilineEntry ();
  uiMultilineEntrySetText (guide, text);
  uiMultilineEntrySetReadOnly (guide, 1);
  uiBoxAppend (hbox, uiControl (guide), 1);

  return uiControl (hbox);
}
