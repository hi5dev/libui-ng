#include "test.h"

#include <ui/button.h>
#include <ui/color_button.h>
#include <ui/label.h>
#include <ui/separator.h>

enum
{
  red,
  green,
  blue,
  yellow,
  white,
  magenta,
  orange,
  purple,
  cyan,
};

static const struct
{
  double r;
  double g;
  double b;
} colors[] = {
  { 1,   0,    0   },
  { 0,   0.5,  0   },
  { 0,   0,    1   },
  { 1,   1,    0   },
  { 1,   1,    1   },
  { 1,   0,    1   },
  { 1,   0.65, 0   },
  { 0.5, 0,    0.5 },
  { 0,   1,    1   },
};

static uiButton *hideOne;
static uiButton *one;
static uiButton *showOne;

static uiControl *
testControl (const char *, const int color)
{
  uiColorButton *b = uiNewColorButton ();
  uiColorButtonSetColor (b, colors[color].r, colors[color].g, colors[color].b, 1.0);
  return uiControl (b);
}

static uiControl *
simpleGrid (void)
{
  uiGrid *g = newGrid ();
  uiGridAppend (g, testControl ("1", red), 0, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("2", green), 1, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("3", blue), 2, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

  uiControl *t4 = testControl ("4", green);
  uiGridAppend (g, t4, 0, 1, 1, 1, 0, uiAlignFill, 1, uiAlignFill);
  uiGridInsertAt (g, testControl ("5", blue), t4, uiAtTrailing, 2, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("6", yellow), -1, 0, 1, 2, 1, uiAlignFill, 0, uiAlignFill);

  return uiControl (g);
}

static uiControl *
boxComparison (void)
{
  uiBox *vbox = newVerticalBox ();
  uiBoxAppend (vbox, uiControl (uiNewLabel ("Above")), 0);
  uiBoxAppend (vbox, uiControl (uiNewHorizontalSeparator ()), 0);

  uiBox *hbox = newHorizontalBox ();
  uiBoxAppend (vbox, uiControl (hbox), 0);
  uiBoxAppend (hbox, testControl ("1", white), 0);
  uiBoxAppend (hbox, uiControl (uiNewLabel ("A label")), 1);
  uiBoxAppend (hbox, testControl ("2", green), 0);
  uiBoxAppend (hbox, uiControl (uiNewLabel ("Another label")), 1);
  uiBoxAppend (hbox, testControl ("3", red), 0);

  uiBoxAppend (vbox, uiControl (uiNewHorizontalSeparator ()), 0);

  uiGrid *g = newGrid ();
  uiBoxAppend (vbox, uiControl (g), 0);
  uiGridAppend (g, testControl ("1", white), 0, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, uiControl (uiNewLabel ("A label")), 1, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("2", green), 2, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, uiControl (uiNewLabel ("Another label")), 3, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("3", red), 4, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

  uiBoxAppend (vbox, uiControl (uiNewHorizontalSeparator ()), 0);
  uiBoxAppend (vbox, uiControl (uiNewLabel ("Below")), 0);
  return uiControl (vbox);
}

static uiControl *
emptyLine (void)
{
  uiGrid *g = newGrid ();

  uiGridAppend (g, testControl ("(0, 0)", red), 0, 0, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
  uiGridAppend (g, testControl ("(0, 1)", blue), 0, 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("(10, 0)", green), 10, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("(10, 1)", magenta), 10, 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

  return uiControl (g);
}

static uiControl *
emptyGrid (void)
{
  uiGrid    *g = newGrid ();
  uiControl *t = testControl ("(0, 0)", red);
  uiGridAppend (g, t, 0, 0, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
  uiControlHide (t);
  return uiControl (g);
}

static uiControl *
spanningGrid (void)
{
  uiGrid *g = newGrid ();

  uiGridAppend (g, testControl ("0", blue), 0, 4, 4, 1, 1, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("1", green), 4, 0, 1, 4, 0, uiAlignFill, 1, uiAlignFill);
  uiGridAppend (g, testControl ("2", red), 3, 3, 1, 1, 1, uiAlignFill, 1, uiAlignFill);
  uiGridAppend (g, testControl ("3", yellow), 0, 3, 2, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("4", orange), 3, 0, 1, 2, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("5", purple), 1, 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("6", white), 0, 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (g, testControl ("7", cyan), 1, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

  return uiControl (g);
}

static void
onHideOne (uiButton *, void *)
{
  uiControlHide (uiControl (one));
}

static void
onShowOne (uiButton *, void *)
{
  uiControlShow (uiControl (one));
}

static void
onHideAll (uiButton *, void *)
{
  uiControlHide (uiControl (hideOne));
  uiControlHide (uiControl (one));
  uiControlHide (uiControl (showOne));
}

static void
onShowAll (uiButton *, void *)
{
  uiControlShow (uiControl (hideOne));
  uiControlShow (uiControl (one));
  uiControlShow (uiControl (showOne));
}

#define AT(x)                                                                                                         \
  static void onInsert##x (uiButton *b, void *data)                                                                   \
  {                                                                                                                   \
    uiGrid *g = uiGrid (data);                                                                                        \
    uiGridInsertAt (g, uiControl (uiNewButton ("Button")), uiControl (b), uiAt##x, 1, 1, 0, uiAlignFill, 0,           \
                    uiAlignFill);                                                                                     \
  }

AT (Leading)
AT (Top)
AT (Trailing)
AT (Bottom)

static uiControl *
assorted (void)
{

  uiGrid *outergrid = newGrid ();
  uiGrid *innergrid = newGrid ();

  one = uiNewButton ("Test");
  uiGridAppend (innergrid, uiControl (one), 1, 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

  hideOne = uiNewButton ("Hide One");
  uiButtonOnClicked (hideOne, onHideOne, NULL);
  uiGridAppend (innergrid, uiControl (hideOne), 0, 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

  showOne = uiNewButton ("Show One");
  uiButtonOnClicked (showOne, onShowOne, NULL);
  uiGridAppend (innergrid, uiControl (showOne), 2, 1, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

  uiButton *b = uiNewButton ("Hide All");
  uiButtonOnClicked (b, onHideAll, NULL);
  uiGridAppend (innergrid, uiControl (b), 1, 0, 1, 1, 0, uiAlignFill, 0, uiAlignFill);

  b = uiNewButton ("Show All");
  uiButtonOnClicked (b, onShowAll, NULL);
  uiGridAppend (innergrid, uiControl (b), 1, 2, 1, 1, 0, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (outergrid, uiControl (innergrid), 0, 0, 1, 1, 1, uiAlignFill, 1, uiAlignFill);

  innergrid = newGrid ();

  b = uiNewButton ("Insert Trailing");
  uiButtonOnClicked (b, onInsertTrailing, innergrid);
  uiGridAppend (innergrid, uiControl (b), 0, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

  b = uiNewButton ("Insert Bottom");
  uiButtonOnClicked (b, onInsertBottom, innergrid);
  uiGridAppend (innergrid, uiControl (b), 1, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

  b = uiNewButton ("Insert Leading");
  uiButtonOnClicked (b, onInsertLeading, innergrid);
  uiGridAppend (innergrid, uiControl (b), 1, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);

  b = uiNewButton ("Insert Top");
  uiButtonOnClicked (b, onInsertTop, innergrid);
  uiGridAppend (innergrid, uiControl (b), 0, 1, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (outergrid, uiControl (innergrid), 1, 0, 1, 1, 1, uiAlignFill, 1, uiAlignFill);

  innergrid = newGrid ();
  uiGridAppend (innergrid, uiControl (uiNewColorButton ()), 0, 0, 1, 1, 1, uiAlignFill, 0, uiAlignFill);
  uiGridAppend (innergrid, uiControl (uiNewColorButton ()), 0, 1, 1, 1, 1, uiAlignStart, 0, uiAlignFill);
  uiGridAppend (innergrid, uiControl (uiNewColorButton ()), 0, 2, 1, 1, 1, uiAlignCenter, 0, uiAlignFill);
  uiGridAppend (innergrid, uiControl (uiNewColorButton ()), 0, 3, 1, 1, 1, uiAlignEnd, 0, uiAlignFill);
  uiGridAppend (outergrid, uiControl (innergrid), 0, 1, 1, 1, 1, uiAlignFill, 1, uiAlignFill);

  innergrid = newGrid ();
  uiGridAppend (innergrid, uiControl (uiNewColorButton ()), 0, 0, 1, 1, 0, uiAlignFill, 1, uiAlignFill);
  uiGridAppend (innergrid, uiControl (uiNewColorButton ()), 1, 0, 1, 1, 0, uiAlignFill, 1, uiAlignStart);
  uiGridAppend (innergrid, uiControl (uiNewColorButton ()), 2, 0, 1, 1, 0, uiAlignFill, 1, uiAlignCenter);
  uiGridAppend (innergrid, uiControl (uiNewColorButton ()), 3, 0, 1, 1, 0, uiAlignFill, 1, uiAlignEnd);
  uiGridAppend (outergrid, uiControl (innergrid), 1, 1, 1, 1, 1, uiAlignFill, 1, uiAlignFill);

  return uiControl (outergrid);
}

static const struct
{
  const char *name;
  uiControl  *(*f) (void);
} pages[] = {
  { "Simple Grid",    simpleGrid    },
  { "Box Comparison", boxComparison },
  { "Empty Line",     emptyLine     },
  { "Empty Grid",     emptyGrid     },
  { "Spanning Grid",  spanningGrid  },
  { "Assorted",       assorted      },
  { NULL,             NULL          },
};

uiTab *
makePage14 (void)
{
  uiTab *page14 = newTab ();

  for (int i = 0; pages[i].name != NULL; i++)
    uiTabAppend (page14, pages[i].name, (*pages[i].f) ());

  return page14;
}
