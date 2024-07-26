#include "control.h"
#include "uipriv.h"
#include "userbugs.h"

#define uiprivControlSignature 0x7569436F

void
uiControlDestroy (uiControl *c)
{
  (*c->Destroy) (c);
}

uintptr_t
uiControlHandle (uiControl *c)
{
  return (*c->Handle) (c);
}

uiControl *
uiControlParent (uiControl *c)
{
  return (*c->Parent) (c);
}

void
uiControlSetParent (uiControl *c, uiControl *parent)
{
  (*c->SetParent) (c, parent);
}

int
uiControlToplevel (uiControl *c)
{
  return (*c->Toplevel) (c);
}

int
uiControlVisible (uiControl *c)
{
  return (*c->Visible) (c);
}

void
uiControlShow (uiControl *c)
{
  (*c->Show) (c);
}

void
uiControlHide (uiControl *c)
{
  (*c->Hide) (c);
}

int
uiControlEnabled (uiControl *c)
{
  return (*c->Enabled) (c);
}

void
uiControlEnable (uiControl *c)
{
  (*c->Enable) (c);
}

void
uiControlDisable (uiControl *c)
{
  (*c->Disable) (c);
}

uiControl *
uiAllocControl (const size_t n, const uint32_t OSsig, const uint32_t typesig, const char *typenamestr)
{
  uiControl *c     = uiprivAlloc (n, typenamestr);
  c->Signature     = uiprivControlSignature;
  c->OSSignature   = OSsig;
  c->TypeSignature = typesig;
  return c;
}

void
uiFreeControl (uiControl *c)
{
  if (uiControlParent (c) != NULL)
    uiprivUserBug ("You cannot destroy a uiControl while it still has a parent. (control: %p)", c);
  uiprivFree (c);
}

void
uiControlVerifySetParent (uiControl *c, uiControl *parent)
{

  if (uiControlToplevel (c))
    uiprivUserBug ("You cannot give a toplevel uiControl a parent. (control: %p)", c);

  uiControl *curParent = uiControlParent (c);

  if (parent != NULL && curParent != NULL)
    uiprivUserBug (
        "You cannot give a uiControl a parent while it already has one. "
        "(control: %p; current parent: %p; new parent: %p)",
        c, curParent, parent);

  if (parent == NULL && curParent == NULL)
    uiprivImplBug ("attempt to double unparent uiControl %p", c);
}

int
uiControlEnabledToUser (uiControl *c)
{
  while (c != NULL)
    {
      if (!uiControlEnabled (c))
        return 0;

      c = uiControlParent (c);
    }

  return 1;
}
