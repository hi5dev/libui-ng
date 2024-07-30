#include "test.h"

uiBox *
makePage7 (void)
{
  uiBox   *page7 = newHorizontalBox ();
  uiGroup *group = makePage7a ();
  uiBoxAppend (page7, uiControl (group), 1);

  uiBox *box2 = newVerticalBox ();
  uiBoxAppend (page7, uiControl (box2), 1);

  group = makePage7b ();
  uiBoxAppend (box2, uiControl (group), 1);

  group = makePage7c ();
  uiBoxAppend (box2, uiControl (group), 1);

  return page7;
}
