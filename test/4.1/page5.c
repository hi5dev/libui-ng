#include "test.h"

#include <ui/button.h>
#include <ui/dialogs.h>
#include <ui/entry.h>
#include <ui/label.h>

static uiWindow *parent;

static void
openFile (uiButton *, void *data)
{
  char *fn = uiOpenFile (parent);
  if (fn == NULL)
    {
      uiLabelSetText (uiLabel (data), "(cancelled)");
    }

  else
    {
      uiLabelSetText (uiLabel (data), fn);
      uiFreeText (fn);
    }
}

static void
openFolder (uiButton *, void *data)
{

  char *fn = uiOpenFolder (parent);

  if (fn == NULL)
    {
      uiLabelSetText (uiLabel (data), "(cancelled)");
    }

  else
    {
      uiLabelSetText (uiLabel (data), fn);
      uiFreeText (fn);
    }
}

static void
saveFile (uiButton *, void *data)
{
  char *fn = uiSaveFile (parent);

  if (fn == NULL)
    {
      uiLabelSetText (uiLabel (data), "(cancelled)");
    }

  else
    {
      uiLabelSetText (uiLabel (data), fn);
      uiFreeText (fn);
    }
}

static uiEntry *title, *description;

static void
msgBox (uiButton *, void *)
{
  char *t = uiEntryText (title);
  char *d = uiEntryText (description);
  uiMsgBox (parent, t, d);
  uiFreeText (d);
  uiFreeText (t);
}

static void
msgBoxError (uiButton *, void *)
{
  char *t = uiEntryText (title);
  char *d = uiEntryText (description);
  uiMsgBoxError (parent, t, d);
  uiFreeText (d);
  uiFreeText (t);
}

void
onFocusChanged (uiWindow *w, void *data)
{
  if (uiWindowFocused (w))
    uiLabelSetText (uiLabel (data), "Window is focused");

  else
    uiLabelSetText (uiLabel (data), "Window is not focused");
}

uiBox *
makePage5 (uiWindow *pw)
{
  uiBox    *hbox;
  uiButton *button;
  uiLabel  *label;

  parent = pw;

  uiBox *page5 = newVerticalBox ();

#define D(n, f)                                                                                                       \
  do                                                                                                                  \
    {                                                                                                                 \
      hbox   = newHorizontalBox ();                                                                                   \
      button = uiNewButton (n);                                                                                       \
      label  = uiNewLabel ("");                                                                                       \
      uiButtonOnClicked (button, f, label);                                                                           \
      uiBoxAppend (hbox, uiControl (button), 0);                                                                      \
      uiBoxAppend (hbox, uiControl (label), 0);                                                                       \
      uiBoxAppend (page5, uiControl (hbox), 0);                                                                       \
    }                                                                                                                 \
  while (0)

  D ("Open File", openFile);
  D ("Open Folder", openFolder);
  D ("Save File", saveFile);
#undef D

  title = uiNewEntry ();
  uiEntrySetText (title, "Title");
  description = uiNewEntry ();
  uiEntrySetText (description, "Description");

  hbox   = newHorizontalBox ();
  button = uiNewButton ("Message Box");
  uiButtonOnClicked (button, msgBox, NULL);
  uiBoxAppend (hbox, uiControl (button), 0);
  uiBoxAppend (hbox, uiControl (title), 0);
  uiBoxAppend (page5, uiControl (hbox), 0);

  hbox   = newHorizontalBox ();
  button = uiNewButton ("Error Box");
  uiButtonOnClicked (button, msgBoxError, NULL);
  uiBoxAppend (hbox, uiControl (button), 0);
  uiBoxAppend (hbox, uiControl (description), 0);
  uiBoxAppend (page5, uiControl (hbox), 0);

  uiLabel *focusLabel = uiNewLabel ("");
  uiBoxAppend (page5, uiControl (focusLabel), 0);

  uiWindowOnFocusChanged (parent, onFocusChanged, focusLabel);

  return page5;
}
