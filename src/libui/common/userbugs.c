#include <ui/userbugs.h>

void
uiUserBugCannotSetParentOnToplevel (const char *type)
{
  uiprivUserBug ("You cannot make a %s a child of another uiControl,", type);
}
