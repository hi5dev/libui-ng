#include "qa.h"

#include <ui/box.h>
#include <ui/button.h>
#include <ui/label.h>

#include <stdio.h>

static void
buttonOnClickedCb (uiButton *, void *data)
{
  char       str[32];
  static int count = 0;
  uiLabel   *label = data;

  sprintf (str, "%d", ++count);
  uiLabelSetText (label, str);
}

const char *
buttonOnClickedGuide ()
{
  return "1.\tYou should see a button with the text `Button`.\n"
         "\tNext to it should a  label displaying `Click count: 0`.\n"
         "\n"
         "2.\tClick the button. The label should now read `Click count: 1`.\n"
         "\n"
         "3.\tClick the button again. The label should now read `Click count: 2`\n";
}

uiControl *
buttonOnClicked ()
{
  uiBox *hbox = uiNewHorizontalBox ();
  uiBoxSetPadded (hbox, 1);

  uiButton *button = uiNewButton ("Button");
  uiBoxAppend (hbox, uiControl (button), 0);

  uiLabel *label = uiNewLabel ("Click count:");
  uiBoxAppend (hbox, uiControl (label), 0);

  label = uiNewLabel ("0");
  uiBoxAppend (hbox, uiControl (label), 0);

  uiButtonOnClicked (button, buttonOnClickedCb, label);

  return uiControl (hbox);
}
