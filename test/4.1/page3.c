#include "test.h"

#include <ui/box.h>
#include <ui/button.h>

static uiBox *
makeSet (const int omit, const int hidden, const int stretch)
{
  uiButton *buttons[4];

  uiBox *hbox = (*newhbox) ();
  uiBoxSetPadded (hbox, 1);
  if (omit != 0)
    {
      buttons[0] = uiNewButton ("First");
      uiBoxAppend (hbox, uiControl (buttons[0]), stretch);
    }

  if (omit != 1)
    {
      buttons[1] = uiNewButton ("Second");
      uiBoxAppend (hbox, uiControl (buttons[1]), stretch);
    }

  if (omit != 2)
    {
      buttons[2] = uiNewButton ("Third");
      uiBoxAppend (hbox, uiControl (buttons[2]), stretch);
    }

  if (omit != 3)
    {
      buttons[3] = uiNewButton ("Fourth");
      uiBoxAppend (hbox, uiControl (buttons[3]), stretch);
    }

  if (hidden != -1)
    uiControlHide (uiControl (buttons[hidden]));

  return hbox;
}

uiBox *
makePage3 (void)
{
  uiBox *hbox;
  int    hidden;

  uiBox *page3 = newVerticalBox ();

  // first the non-stretchy type
  for (hidden = 0; hidden < 4; hidden++)
    {
      // these two must stay unpadded as well, otherwise the test isn't meaningful
      uiBox *hbox2 = (*newhbox) ();
      uiBox *vbox  = (*newvbox) ();

      // reference set
      hbox = makeSet (hidden, -1, 0);
      uiBoxAppend (vbox, uiControl (hbox), 0);

      // real thing
      hbox = makeSet (-1, hidden, 0);
      uiBoxAppend (vbox, uiControl (hbox), 0);

      // pack vbox in
      uiBoxAppend (hbox2, uiControl (vbox), 0);

      // and have a button in there for showing right margins
      uiBoxAppend (hbox2, uiControl (uiNewButton ("Right Margin Test")), 1);
      uiBoxAppend (page3, uiControl (hbox2), 0);
    }

  // then the stretchy type
  for (hidden = 0; hidden < 4; hidden++)
    {
      hbox = makeSet (-1, hidden, 1);
      uiBoxAppend (page3, uiControl (hbox), 0);
    }

  return page3;
}
