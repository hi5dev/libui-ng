#include <ui/init.h>
#include <ui/label.h>
#include <ui/main.h>
#include <ui/window.h>

#include <stdio.h>

int
onClosing (uiWindow *, void *)
{
  uiQuit ();
  return 1;
}

int
main (void)
{
  uiInitOptions o = { 0 };

  const char *err = uiInit (&o);
  if (err != NULL)
    {
      fprintf (stderr, "Error initializing libui-ng: %s\n", err);
      uiFreeInitError (err);
      return 1;
    }

  // Create a new window
  uiWindow *w = uiNewWindow ("Hello World!", 300, 30, 0);
  uiWindowOnClosing (w, onClosing, NULL);

  uiLabel *l = uiNewLabel ("Hello, World!");
  uiWindowSetChild (w, uiControl (l));

  uiControlShow (uiControl (w));
  uiMain ();
  uiUninit ();
  return 0;
}
