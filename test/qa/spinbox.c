#include "qa.h"

#include <stdio.h>
#include <ui/box.h>
#include <ui/control.h>
#include <ui/label.h>
#include <ui/spinbox.h>

static void
spinboxOnChangedCb (uiSpinbox *, void *data)
{
  char       str[32];
  uiLabel   *label = data;
  static int count = 0;

  (void)sprintf (str, "%d", ++count);
  uiLabelSetText (label, str);
}

const char *
spinboxOnChangedGuide ()
{
  return "1.\tYou should see a spinbox consisting of a text entry and two buttons.\n"
         "\tThe number within the text entry should read `0` and the label\n"
         "\tshould read `Changed count: 0`.\n"
         "\n"
         "2.\tClick the increase button (plus/arrow up).\n"
         "\tThe number within the text entry should read `1` and the label\n"
         "\tshould read `Changed count: 1`.\n"
         "\n"
         "2.\tClick the decrease button (minus/arrow down).\n"
         "\tThe number within the text entry should read `0` and the label\n"
         "\tshould read `Changed count: 2`.\n"
         "\n"
         "3.\tFocus the text entry, enter the number `5` and hit enter.\n"
         "\tThe number within the text entry should read `5` and the label\n"
         "\tshould read `Changed count: 3`.\n"
         "\n";
}

uiControl *
spinboxOnChanged ()
{
  uiBox *hbox = uiNewHorizontalBox ();
  uiBoxSetPadded (hbox, 1);

  uiSpinbox *spinbox = uiNewSpinbox (0, 10);
  uiBoxAppend (hbox, uiControl (spinbox), 0);

  uiLabel *label = uiNewLabel ("Changed count:");
  uiBoxAppend (hbox, uiControl (label), 0);

  label = uiNewLabel ("0");
  uiBoxAppend (hbox, uiControl (label), 0);

  uiSpinboxOnChanged (spinbox, spinboxOnChangedCb, label);

  return uiControl (hbox);
}
