#include "test.h"

#include <ui/init.h>
#include <ui/main.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uiBox *mainBox;
uiTab *mainTab;

uiBox *(*newhbox) (void);
uiBox *(*newvbox) (void);

static int
onClosing (uiWindow *, void *)
{
  (void)printf ("%s\n", __FUNCTION__);
  uiQuit ();
  return 1;
}

static int
onShouldQuit (void *data)
{
  (void)printf ("%s\n", __FUNCTION__);

  if (uiMenuItemChecked (shouldQuitItem))
    {
      uiControlDestroy (uiControl (data));
      return 1;
    }

  return 0;
}

int
main (const int argc, char *argv[])
{
  uiInitOptions o;
  int           nomenus     = 0;
  int           startspaced = 0;
  int           steps       = 0;

  newhbox = uiNewHorizontalBox;
  newvbox = uiNewVerticalBox;

  memset (&o, 0, sizeof (uiInitOptions));
  for (int i = 1; i < argc; i++)
    if (strcmp (argv[i], "nomenus") == 0)
      {
        nomenus = 1;
      }

    else if (strcmp (argv[i], "startspaced") == 0)
      {
        startspaced = 1;
      }

    else if (strcmp (argv[i], "swaphv") == 0)
      {
        newhbox = uiNewVerticalBox;
        newvbox = uiNewHorizontalBox;
      }

    else if (strcmp (argv[i], "steps") == 0)
      {
        steps = 1;
      }

    else
      {
        fprintf (stderr, "%s: unrecognized option %s\n", argv[0], argv[i]);
        return 1;
      }

  const char *err = uiInit (&o);
  if (err != NULL)
    {
      fprintf (stderr, "error initializing ui: %s\n", err);
      uiFreeInitError (err);
      return 1;
    }

  if (!nomenus)
    initMenus ();

  uiWindow *w = newWindow ("Main Window", 320, 240, 1);
  uiWindowOnClosing (w, onClosing, NULL);
  printf ("main window %p\n", (void *)w);

  uiOnShouldQuit (onShouldQuit, w);

  mainBox = newHorizontalBox ();
  uiWindowSetChild (w, uiControl (mainBox));

  uiTab *outerTab = newTab ();
  uiBoxAppend (mainBox, uiControl (outerTab), 1);

  mainTab = newTab ();
  uiTabAppend (outerTab, "Pages 1-5", uiControl (mainTab));

  // page 1 uses page 2's uiGroup
  uiBox *page2 = makePage2 ();

  makePage1 (w);
  uiTabAppend (mainTab, "Page 1", uiControl (page1));

  uiTabAppend (mainTab, "Page 2", uiControl (page2));

  uiTabAppend (mainTab, "Empty Page", uiControl (uiNewHorizontalBox ()));

  uiBox *page3 = makePage3 ();
  uiTabAppend (mainTab, "Page 3", uiControl (page3));

  uiBox *page4 = makePage4 ();
  uiTabAppend (mainTab, "Page 4", uiControl (page4));

  uiBox *page5 = makePage5 (w);
  uiTabAppend (mainTab, "Page 5", uiControl (page5));

  uiTab *innerTab = newTab ();
  uiTabAppend (outerTab, "Pages 6-10", uiControl (innerTab));

  uiBox *page6 = makePage6 ();
  uiTabAppend (innerTab, "Page 6", uiControl (page6));

  uiBox *page7 = makePage7 ();
  uiTabAppend (innerTab, "Page 7", uiControl (page7));

  innerTab = newTab ();
  uiTabAppend (outerTab, "Pages 11-15", uiControl (innerTab));

  uiBox *page12 = makePage12 ();
  uiTabAppend (innerTab, "Page 12", uiControl (page12));

  uiBox *page13 = makePage13 ();
  uiTabAppend (innerTab, "Page 13", uiControl (page13));

  uiTab *page14 = makePage14 ();
  uiTabAppend (innerTab, "Page 14", uiControl (page14));

  uiBox *page15 = makePage15 (w);
  uiTabAppend (innerTab, "Page 15", uiControl (page15));

  innerTab = newTab ();
  uiTabAppend (outerTab, "Pages 16-?", uiControl (innerTab));

  uiBox *page16 = makePage16 ();
  uiTabAppend (innerTab, "Page 16", uiControl (page16));

  uiBox *page17 = makePage17 ();
  uiTabAppend (innerTab, "Page 17", uiControl (page17));

  if (startspaced)
    setSpaced (1);

  uiControlShow (uiControl (w));
  if (!steps)
    {
      uiMain ();
    }

  else
    {
      uiMainSteps ();
      while (uiMainStep (1))
        ;
    }

  printf ("after uiMain()\n");

  freePage17 ();
  freePage16 ();

  uiUninit ();

  printf ("after uiUninit()\n");

  return 0;
}
