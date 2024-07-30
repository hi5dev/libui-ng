#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ui/box.h>
#include <ui/button.h>
#include <ui/init.h>
#include <ui/main.h>
#include <ui/multiline_entry.h>
#include <ui/window.h>

uiMultilineEntry *e;

int
sayTime (void *)
{
  const time_t t = time (NULL);
  const char  *s = ctime (&t);

  uiMultilineEntryAppend (e, s);
  return 1;
}

int
onClosing (uiWindow *, void *)
{
  uiQuit ();
  return 1;
}

void
saySomething (uiButton *, void *)
{
  uiMultilineEntryAppend (e, "Saying something\n");
}

int
main (void)
{
  uiInitOptions o = { 0 };

  if (uiInit (&o) != NULL)
    abort ();

  uiWindow *w = uiNewWindow ("Hello", 320, 240, 0);
  uiWindowSetMargined (w, 1);

  uiBox *b = uiNewVerticalBox ();
  uiBoxSetPadded (b, 1);
  uiWindowSetChild (w, uiControl (b));

  e = uiNewMultilineEntry ();
  uiMultilineEntrySetReadOnly (e, 1);

  uiButton *btn = uiNewButton ("Say Something");
  uiButtonOnClicked (btn, saySomething, NULL);
  uiBoxAppend (b, uiControl (btn), 0);

  uiBoxAppend (b, uiControl (e), 1);

  uiTimer (1000, sayTime, NULL);

  uiWindowOnClosing (w, onClosing, NULL);
  uiControlShow (uiControl (w));
  uiMain ();
  return 0;
}
