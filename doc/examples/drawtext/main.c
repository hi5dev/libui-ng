#include <ui/area.h>
#include <ui/box.h>
#include <ui/checkbox.h>
#include <ui/combobox.h>
#include <ui/font_button.h>
#include <ui/form.h>
#include <ui/init.h>
#include <ui/main.h>
#include <ui/window.h>

#include <stdio.h>
#include <string.h>

uiWindow     *mainwin;
uiArea       *area;
uiAreaHandler handler;
uiFontButton *fontButton;
uiCombobox   *alignment;
uiCheckbox   *systemFont;

uiAttributedString *attrstr;

static void
appendWithAttribute (const char *what, uiAttribute *attr, uiAttribute *attr2)
{

  const size_t start = uiAttributedStringLen (attrstr);
  const size_t end   = start + strlen (what);
  uiAttributedStringAppendUnattributed (attrstr, what);
  uiAttributedStringSetAttribute (attrstr, attr, start, end);
  if (attr2 != NULL)
    uiAttributedStringSetAttribute (attrstr, attr2, start, end);
}

static void
makeAttributedString (void)
{

  attrstr = uiNewAttributedString (
      "Drawing strings with libui is done with the uiAttributedString and uiDrawTextLayout objects.\n"
      "uiAttributedString lets you have a variety of attributes: ");

  uiAttribute *attr = uiNewFamilyAttribute ("Courier New");
  appendWithAttribute ("font family", attr, NULL);
  uiAttributedStringAppendUnattributed (attrstr, ", ");

  attr = uiNewSizeAttribute (18);
  appendWithAttribute ("font size", attr, NULL);
  uiAttributedStringAppendUnattributed (attrstr, ", ");

  attr = uiNewWeightAttribute (uiTextWeightBold);
  appendWithAttribute ("font weight", attr, NULL);
  uiAttributedStringAppendUnattributed (attrstr, ", ");

  attr = uiNewItalicAttribute (uiTextItalicItalic);
  appendWithAttribute ("font italicness", attr, NULL);
  uiAttributedStringAppendUnattributed (attrstr, ", ");

  attr = uiNewStretchAttribute (uiTextStretchCondensed);
  appendWithAttribute ("font stretch", attr, NULL);
  uiAttributedStringAppendUnattributed (attrstr, ", ");

  attr = uiNewColorAttribute (0.75, 0.25, 0.5, 0.75);
  appendWithAttribute ("text color", attr, NULL);
  uiAttributedStringAppendUnattributed (attrstr, ", ");

  attr = uiNewBackgroundAttribute (0.5, 0.5, 0.25, 0.5);
  appendWithAttribute ("text background color", attr, NULL);
  uiAttributedStringAppendUnattributed (attrstr, ", ");

  attr = uiNewUnderlineAttribute (uiUnderlineSingle);
  appendWithAttribute ("underline style", attr, NULL);
  uiAttributedStringAppendUnattributed (attrstr, ", ");

  uiAttributedStringAppendUnattributed (attrstr, "and ");
  attr               = uiNewUnderlineAttribute (uiUnderlineDouble);
  uiAttribute *attr2 = uiNewUnderlineColorAttribute (uiUnderlineColorCustom, 1.0, 0.0, 0.5, 1.0);
  appendWithAttribute ("underline color", attr, attr2);
  uiAttributedStringAppendUnattributed (attrstr, ". ");

  uiAttributedStringAppendUnattributed (attrstr, "Furthermore, there are attributes allowing for ");
  attr  = uiNewUnderlineAttribute (uiUnderlineSuggestion);
  attr2 = uiNewUnderlineColorAttribute (uiUnderlineColorSpelling, 0, 0, 0, 0);
  appendWithAttribute ("special underlines for indicating spelling errors", attr, attr2);
  uiAttributedStringAppendUnattributed (attrstr, " (and other types of errors) ");

  uiAttributedStringAppendUnattributed (attrstr,
                                        "and control over OpenType features such as ligatures (for instance, ");
  uiOpenTypeFeatures *otf = uiNewOpenTypeFeatures ();
  uiOpenTypeFeaturesAdd (otf, 'l', 'i', 'g', 'a', 0);
  attr = uiNewFeaturesAttribute (otf);
  appendWithAttribute ("afford", attr, NULL);
  uiAttributedStringAppendUnattributed (attrstr, " vs. ");
  uiOpenTypeFeaturesAdd (otf, 'l', 'i', 'g', 'a', 1);
  attr = uiNewFeaturesAttribute (otf);
  appendWithAttribute ("afford", attr, NULL);
  uiFreeOpenTypeFeatures (otf);
  uiAttributedStringAppendUnattributed (attrstr, ").\n");

  uiAttributedStringAppendUnattributed (attrstr,
                                        "Use the controls opposite to the text to control properties of the text.");
}

static void
handlerDraw (uiAreaHandler *, uiArea *, uiAreaDrawParams *p)
{
  uiFontDescriptor       defaultFont;
  uiDrawTextLayoutParams params;
  const int              useSystemFont = uiCheckboxChecked (systemFont);

  params.String = attrstr;
  if (useSystemFont)
    uiLoadControlFont (&defaultFont);
  else
    uiFontButtonFont (fontButton, &defaultFont);
  params.DefaultFont           = &defaultFont;
  params.Width                 = p->AreaWidth;
  params.Align                 = (uiDrawTextAlign)uiComboboxSelected (alignment);
  uiDrawTextLayout *textLayout = uiDrawNewTextLayout (&params);
  uiDrawText (p->Context, textLayout, 0, 0);
  uiDrawFreeTextLayout (textLayout);

  // TODO RENAME?
  uiFreeFontButtonFont (&defaultFont);
}

static void
handlerMouseEvent (uiAreaHandler *a, uiArea *area, uiAreaMouseEvent *e)
{
  // do nothing
}

static void
handlerMouseCrossed (uiAreaHandler *ah, uiArea *a, int left)
{
  // do nothing
}

static void
handlerDragBroken (uiAreaHandler *ah, uiArea *a)
{
  // do nothing
}

static int
handlerKeyEvent (uiAreaHandler *ah, uiArea *a, uiAreaKeyEvent *e)
{
  // reject all keys
  return 0;
}

static void
onFontChanged (uiFontButton *b, void *data)
{
  uiAreaQueueRedrawAll (area);
}

static void
onComboboxSelected (uiCombobox *b, void *data)
{
  uiAreaQueueRedrawAll (area);
}

static void
onCheckboxToggled (uiCheckbox *b, void *data)
{
  uiAreaQueueRedrawAll (area);
}

static int
onClosing (uiWindow *, void *)
{
  uiControlDestroy (uiControl (mainwin));
  uiQuit ();
  return 0;
}

static int
shouldQuit (void *)
{
  uiControlDestroy (uiControl (mainwin));
  return 1;
}

int
main (void)
{
  uiInitOptions o;

  handler.Draw         = handlerDraw;
  handler.MouseEvent   = handlerMouseEvent;
  handler.MouseCrossed = handlerMouseCrossed;
  handler.DragBroken   = handlerDragBroken;
  handler.KeyEvent     = handlerKeyEvent;

  memset (&o, 0, sizeof (uiInitOptions));
  const char *err = uiInit (&o);
  if (err != NULL)
    {
      fprintf (stderr, "error initializing ui: %s\n", err);
      uiFreeInitError (err);
      return 1;
    }

  uiOnShouldQuit (shouldQuit, NULL);

  makeAttributedString ();

  mainwin = uiNewWindow ("libui Text-Drawing Example", 640, 480, 1);
  uiWindowSetMargined (mainwin, 1);
  uiWindowOnClosing (mainwin, onClosing, NULL);

  uiBox *hbox = uiNewHorizontalBox ();
  uiBoxSetPadded (hbox, 1);
  uiWindowSetChild (mainwin, uiControl (hbox));

  uiBox *vbox = uiNewVerticalBox ();
  uiBoxSetPadded (vbox, 1);
  uiBoxAppend (hbox, uiControl (vbox), 0);

  fontButton = uiNewFontButton ();
  uiFontButtonOnChanged (fontButton, onFontChanged, NULL);
  uiBoxAppend (vbox, uiControl (fontButton), 0);

  uiForm *form = uiNewForm ();
  uiFormSetPadded (form, 1);
  // TODO on OS X if this is set to 1 then the window can't resize; does the form not have the concept of stretchy
  // trailing space?
  uiBoxAppend (vbox, uiControl (form), 0);

  alignment = uiNewCombobox ();
  // note that the items match with the values of the uiDrawTextAlign values
  uiComboboxAppend (alignment, "Left");
  uiComboboxAppend (alignment, "Center");
  uiComboboxAppend (alignment, "Right");
  uiComboboxSetSelected (alignment, 0); // start with left alignment
  uiComboboxOnSelected (alignment, onComboboxSelected, NULL);
  uiFormAppend (form, "Alignment", uiControl (alignment), 0);

  systemFont = uiNewCheckbox ("");
  uiCheckboxOnToggled (systemFont, onCheckboxToggled, NULL);
  uiFormAppend (form, "System Font", uiControl (systemFont), 0);

  area = uiNewArea (&handler);
  uiBoxAppend (hbox, uiControl (area), 1);

  uiControlShow (uiControl (mainwin));
  uiMain ();
  uiFreeAttributedString (attrstr);
  uiUninit ();
  return 0;
}
