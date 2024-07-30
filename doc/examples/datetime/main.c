#include <ui/button.h>
#include <ui/date_time_picker.h>
#include <ui/grid.h>
#include <ui/init.h>
#include <ui/label.h>
#include <ui/main.h>
#include <ui/window.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

uiDateTimePicker *dtboth, *dtdate, *dttime;

const char *
timeFormat (const uiDateTimePicker *d)
{
  const char *fmt;

  if (d == dtboth)
    fmt = "%c";
  else if (d == dtdate)
    fmt = "%x";
  else if (d == dttime)
    fmt = "%X";
  else
    fmt = "";
  return fmt;
}

void
onChanged (uiDateTimePicker *d, void *data)
{
  struct tm time;
  char      buf[64];

  uiDateTimePickerTime (d, &time);
  strftime (buf, sizeof (buf), timeFormat (d), &time);
  uiLabelSetText (uiLabel (data), buf);
}

void
onClicked (uiButton *, void *data)
{
  struct tm tmbuf;

  const intptr_t now = (intptr_t)data;

  time_t t = 0;

  if (now)
    t = time (NULL);

  tmbuf = *localtime (&t);

  if (now)
    {
      uiDateTimePickerSetTime (dtdate, &tmbuf);
      uiDateTimePickerSetTime (dttime, &tmbuf);
    }
  else
    uiDateTimePickerSetTime (dtboth, &tmbuf);
}

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
      fprintf (stderr, "error initializing ui: %s\n", err);
      uiFreeInitError (err);
      return 1;
    }

  uiWindow *w = uiNewWindow ("Date / Time", 320, 240, 0);
  uiWindowSetMargined (w, 1);

  uiGrid *g = uiNewGrid ();
  uiGridSetPadded (g, 1);
  uiWindowSetChild (w, uiControl (g));

  dtboth = uiNewDateTimePicker ();
  dtdate = uiNewDatePicker ();
  dttime = uiNewTimePicker ();

  uiGridAppend (g, uiControl (dtboth), 0, 0, 2, 1, 1, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, uiControl (dtdate), 0, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, uiControl (dttime), 1, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

  uiLabel *l = uiNewLabel ("");
  uiGridAppend (g, uiControl (l), 0, 2, 2, 1, 1, uiAlignCenter, 0, uiAlignFill);
  uiDateTimePickerOnChanged (dtboth, onChanged, l);
  l = uiNewLabel ("");
  uiGridAppend (g, uiControl (l), 0, 3, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
  uiDateTimePickerOnChanged (dtdate, onChanged, l);
  l = uiNewLabel ("");
  uiGridAppend (g, uiControl (l), 1, 3, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
  uiDateTimePickerOnChanged (dttime, onChanged, l);

  uiButton *b = uiNewButton ("Now");
  uiButtonOnClicked (b, onClicked, (void *)1);
  uiGridAppend (g, uiControl (b), 0, 4, 1, 1, 1, uiAlignFill, 1, uiAlignEnd);
  b = uiNewButton ("Unix epoch");
  uiButtonOnClicked (b, onClicked, (void *)0);
  uiGridAppend (g, uiControl (b), 1, 4, 1, 1, 1, uiAlignFill, 1, uiAlignEnd);

  uiWindowOnClosing (w, onClosing, NULL);
  uiControlShow (uiControl (w));
  uiMain ();
  return 0;
}
