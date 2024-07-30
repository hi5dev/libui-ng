#pragma once

#include <windef.h>

#include <ui/menu.h>

#define grow 32

typedef enum MenuType
{
  typeRegular,
  typeCheckbox,
  typeQuit,
  typePreferences,
  typeAbout,
  typeSeparator,
} MenuType;

extern HMENU makeMenubar ();

extern const uiMenuItem *menuIDToItem (UINT_PTR);

extern void runMenuEvent (WORD, uiWindow *);

extern void freeMenubar (HMENU);

extern void uninitMenus ();
