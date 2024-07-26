#include "test.h"

#include <stdio.h>
#include <ui/button.h>
#include <ui/checkbox.h>
#include <ui/color_button.h>
#include <ui/date_time_picker.h>
#include <ui/editable_combobox.h>
#include <ui/entry.h>
#include <ui/font_button.h>
#include <ui/label.h>
#include <ui/multiline_entry.h>
#include <ui/progressbar.h>
#include <ui/radio_buttons.h>
#include <ui/separator.h>
#include <ui/slider.h>
#include <ui/spinbox.h>

static int
winClose (uiWindow *, void *)
{
  return 1;
}

static void openTestWindow(uiBox *(*mkf)(void))
{
  uiWindow *w = uiNewWindow ("Test", 100, 100, 0);
  uiWindowOnClosing (w, winClose, NULL);
  uiWindowSetMargined (w, 1);

  uiBox *b = (*mkf) ();
  uiWindowSetChild (w, uiControl (b));

#define BA(x) uiBoxAppend(b, uiControl(x), 0)
  BA (uiNewButton (""));
  BA (uiNewCheckbox (""));
  BA (uiNewEntry ());
  BA (uiNewLabel (""));
  BA (uiNewSpinbox (0, 100));
  BA (uiNewProgressBar ());
  BA (uiNewSlider (0, 100));
  BA (uiNewHorizontalSeparator ());

  uiCombobox *c = uiNewCombobox ();
  uiComboboxAppend (c, "");
  BA (c);

  uiEditableCombobox *e = uiNewEditableCombobox ();
  uiEditableComboboxAppend (e, "");
  BA (e);

  uiRadioButtons *r = uiNewRadioButtons ();
  uiRadioButtonsAppend (r, "");
  BA (r);

  BA (uiNewDateTimePicker ());
  BA (uiNewDatePicker ());
  BA (uiNewTimePicker ());
  BA (uiNewMultilineEntry ());

  BA (uiNewFontButton ());
  BA (uiNewColorButton ());
  BA (uiNewPasswordEntry ());
  BA (uiNewSearchEntry ());
  BA (uiNewVerticalSeparator ());

  uiControlShow (uiControl (w));
#undef BA
}

static void
hButtonClicked (uiButton *, void *)
{
  openTestWindow (uiNewHorizontalBox);
}

static void
vButtonClicked (uiButton *, void *)
{
  openTestWindow (uiNewVerticalBox);
}

static void
entryChanged (uiEntry *e, const void *data)
{

  char *text = uiEntryText (e);
  (void)printf ("%s entry changed: %s\n", (const char *)data, text);
  uiFreeText (text);
}

static void
showHide (uiButton *, void *data)
{
  uiControl *c = uiControl (data);

  if (uiControlVisible (c))
    uiControlHide (c);

  else
    uiControlShow (c);
}

static void
setIndeterminate (uiButton *, void *data)
{
  uiProgressBar *p = uiProgressBar (data);

  int value = uiProgressBarValue (p);

  if (value == -1)
    value = 50;

  else
    value = -1;

  uiProgressBarSetValue (p, value);
}

static void
deleteFirst (uiButton *, void *data)
{
  uiForm *f = uiForm (data);

  uiFormDelete (f, 0);
}

uiBox *
makePage13 (void)
{
  uiBox *page13 = newVerticalBox ();

  uiRadioButtons *rb = uiNewRadioButtons ();
  uiRadioButtonsAppend (rb, "Item 1");
  uiRadioButtonsAppend (rb, "Item 2");
  uiRadioButtonsAppend (rb, "Item 3");
  uiBoxAppend (page13, uiControl (rb), 0);

  rb = uiNewRadioButtons ();
  uiRadioButtonsAppend (rb, "Item A");
  uiRadioButtonsAppend (rb, "Item B");
  uiBoxAppend (page13, uiControl (rb), 0);

  uiButton *b = uiNewButton ("Horizontal");
  uiButtonOnClicked (b, hButtonClicked, NULL);
  uiBoxAppend (page13, uiControl (b), 0);

  b = uiNewButton ("Vertical");
  uiButtonOnClicked (b, vButtonClicked, NULL);
  uiBoxAppend (page13, uiControl (b), 0);

  uiForm *f = newForm ();

  uiEntry *e = uiNewPasswordEntry ();
  uiEntryOnChanged (e, entryChanged, "password");
  uiFormAppend (f, "Password Entry", uiControl (e), 0);

  e = uiNewSearchEntry ();
  uiEntryOnChanged (e, entryChanged, "search");
  uiFormAppend (f, "Search Box", uiControl (e), 0);
  uiFormAppend (f, "MLE", uiControl (uiNewMultilineEntry ()), 1);

  uiProgressBar *p = uiNewProgressBar ();
  uiProgressBarSetValue (p, 50);
  uiBoxAppend (page13, uiControl (p), 0);

  b = uiNewButton ("Toggle Indeterminate");
  uiButtonOnClicked (b, setIndeterminate, p);
  uiBoxAppend (page13, uiControl (b), 0);

  b = uiNewButton ("Show/Hide");
  uiButtonOnClicked (b, showHide, e);
  uiBoxAppend (page13, uiControl (b), 0);

  b = uiNewButton ("Delete First");
  uiButtonOnClicked (b, deleteFirst, f);
  uiBoxAppend (page13, uiControl (b), 0);
  uiBoxAppend (page13, uiControl (f), 1);

  return page13;
}
