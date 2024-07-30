#include "uipriv.h"

#include <ui/table.h>

void
uiFreeTableSelection (uiTableSelection *s)
{
  if (s->Rows != NULL)
    uiprivFree (s->Rows);

  uiprivFree (s);
}
