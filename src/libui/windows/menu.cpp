#include <windows.h>

#include "menu.h"
#include "debug.h"
#include "utf16.h"

#include <uipriv.h>

#include <ui/main.h>
#include <ui/menu.h>
#include <ui/userbugs.h>

static uiMenu **menus          = nullptr;
static size_t   len            = 0;
static size_t   cap            = 0;
static BOOL     menusFinalized = FALSE;
static WORD     curID          = 100;
static BOOL     hasQuit        = FALSE;
static BOOL     hasPreferences = FALSE;
static BOOL     hasAbout       = FALSE;

struct uiMenu
{
  WCHAR       *name;
  uiMenuItem **items;
  size_t       len;
  size_t       cap;
};

struct uiMenuItem
{
  WCHAR *name;
  int    type;
  WORD   id;
  void   (*onClicked) (uiMenuItem *, uiWindow *, void *);
  void  *onClickedData;
  BOOL   disabled; // template for new instances; kept in sync with everything else
  BOOL   checked;
  HMENU *hmenus;
  size_t len;
  size_t cap;
};

static void
sync (const uiMenuItem *item)
{
  MENUITEMINFOW mi;

  ZeroMemory (&mi, sizeof (MENUITEMINFOW));
  mi.cbSize = sizeof (MENUITEMINFOW);
  mi.fMask  = MIIM_STATE;

  if (item->disabled != 0)
    mi.fState |= MFS_DISABLED;

  if (item->checked != 0)
    mi.fState |= MFS_CHECKED;

  for (size_t i = 0; i < item->len; i++)
    if (SetMenuItemInfo (item->hmenus[i], item->id, FALSE, &mi) == 0)
      (void)logLastError (L"error synchronizing menu items");
}

static void
defaultOnClicked (uiMenuItem *, uiWindow *, void *)
{
  // no-op
}

static void
onQuitClicked (uiMenuItem *, uiWindow *, void *)
{
  if (uiprivShouldQuit () != 0)
    uiQuit ();
}

void
uiMenuItemEnable (uiMenuItem *m)
{
  m->disabled = FALSE;
  sync (m);
}

void
uiMenuItemDisable (uiMenuItem *m)
{
  m->disabled = TRUE;
  sync (m);
}

void
uiMenuItemOnClicked (uiMenuItem *i, void (*f) (uiMenuItem *, uiWindow *, void *), void *data)
{
  if (i->type == typeQuit)
    uiprivUserBug ("You can not call uiMenuItemOnClicked() on a Quit item; use uiOnShouldQuit() instead.");

  i->onClicked     = f;
  i->onClickedData = data;
}

int
uiMenuItemChecked (const uiMenuItem *m)
{
  return m->checked;
}

void
uiMenuItemSetChecked (uiMenuItem *m, const int checked)
{
  m->checked = FALSE;

  if (checked != 0)
    m->checked = TRUE;

  sync (m);
}

static uiMenuItem *
newItem (uiMenu *m, const int type, const char *name)
{
  if (menusFinalized != 0)
    uiprivUserBug ("You can not create a new menu item after menus have been finalized.");

  if (m->len >= m->cap)
    {
      m->cap += grow;
      m->items
          = static_cast<uiMenuItem **> (uiprivRealloc (m->items, // NOLINT(*-multi-level-implicit-pointer-conversion)
                                                       m->cap * sizeof (uiMenuItem *), "uiMenuitem *[]"));
    }

  auto *item = uiprivNew (uiMenuItem);

  m->items[m->len] = item;
  m->len++;

  item->type = type;
  switch (item->type)
    {
    case typeQuit:
      item->name = toUTF16 ("Quit");
      break;

    case typePreferences:
      item->name = toUTF16 ("Preferences...");
      break;

    case typeAbout:
      item->name = toUTF16 ("About");
      break;

    case typeSeparator:
      break;

    default:
      item->name = toUTF16 (name);
      break;
    }

  if (item->type != typeSeparator)
    {
      item->id = curID;
      curID++;
    }

  if (item->type == typeQuit)
    {
      item->onClicked     = onQuitClicked;
      item->onClickedData = nullptr;
    }
  else
    uiMenuItemOnClicked (item, defaultOnClicked, nullptr);

  return item;
}

uiMenuItem *
uiMenuAppendItem (uiMenu *m, const char *name)
{
  return newItem (m, typeRegular, name);
}

uiMenuItem *
uiMenuAppendCheckItem (uiMenu *m, const char *name)
{
  return newItem (m, typeCheckbox, name);
}

uiMenuItem *
uiMenuAppendQuitItem (uiMenu *m)
{
  if (hasQuit != 0)
    uiprivUserBug ("You can not have multiple Quit menu items in a program.");

  hasQuit = TRUE;

  newItem (m, typeSeparator, nullptr);

  return newItem (m, typeQuit, nullptr);
}

uiMenuItem *
uiMenuAppendPreferencesItem (uiMenu *m)
{
  if (hasPreferences != 0)
    uiprivUserBug ("You can not have multiple Preferences menu items in a program.");

  hasPreferences = TRUE;

  newItem (m, typeSeparator, nullptr);

  return newItem (m, typePreferences, nullptr);
}

uiMenuItem *
uiMenuAppendAboutItem (uiMenu *m)
{
  if (hasAbout != 0)
    uiprivUserBug ("You can not have multiple About menu items in a program.");

  hasAbout = TRUE;

  newItem (m, typeSeparator, nullptr);

  return newItem (m, typeAbout, nullptr);
}

void
uiMenuAppendSeparator (uiMenu *m)
{
  newItem (m, typeSeparator, nullptr);
}

uiMenu *
uiNewMenu (const char *name)
{
  if (menusFinalized != 0)
    uiprivUserBug ("You can not create a new menu after menus have been finalized.");

  if (len >= cap)
    {
      cap += grow;
      menus = static_cast<uiMenu **> (uiprivRealloc (menus, // NOLINT(*-multi-level-implicit-pointer-conversion)
                                                     cap * sizeof (uiMenu *), "uiMenu *[]"));
    }

  auto *const m = uiprivNew (uiMenu);

  menus[len] = m;
  len++;

  m->name = toUTF16 (name);

  return m;
}

static void
appendMenuItem (const HMENU menu, uiMenuItem *item)
{

  UINT uFlags = MF_SEPARATOR;
  if (item->type != typeSeparator)
    {
      uFlags = MF_STRING;

      if (item->disabled != 0)
        uFlags |= MF_DISABLED | MF_GRAYED;

      if (item->checked != 0)
        uFlags |= MF_CHECKED;
    }
  if (AppendMenuW (menu, uFlags, item->id, item->name) == 0)
    (void)logLastError (L"error appending menu item");

  if (item->len >= item->cap)
    {
      item->cap += grow;
      item->hmenus
          = static_cast<HMENU *> (uiprivRealloc (item->hmenus, // NOLINT(*-multi-level-implicit-pointer-conversion)
                                                 item->cap * sizeof (HMENU), "HMENU[]"));
    }
  item->hmenus[item->len] = menu;
  item->len++;
}

static HMENU
makeMenu (const uiMenu *m)
{
  const HMENU menu = CreatePopupMenu ();

  if (menu == nullptr)
    (void)logLastError (L"error creating menu");

  for (size_t i = 0; i < m->len; i++)
    appendMenuItem (menu, m->items[i]);

  return menu;
}

HMENU
makeMenubar (void)
{

  menusFinalized = TRUE;

  const HMENU menubar = CreateMenu ();
  if (menubar == nullptr)
    (void)logLastError (L"error creating menubar");

  for (size_t i = 0; i < len; i++)
    {
      HMENU menu = makeMenu (menus[i]);
      if (AppendMenuW (menubar, MF_POPUP | MF_STRING, reinterpret_cast<UINT_PTR> (menu), menus[i]->name) == 0)
        (void)logLastError (L"error appending menu to menubar");
    }

  return menubar;
}

void
runMenuEvent (const WORD id, uiWindow *w)
{
  uiMenuItem *item = nullptr;

  // this isn't optimal, but it works, and it should be just fine for most cases
  for (size_t i = 0; i < len; i++)
    {
      const uiMenu *m = menus[i];
      for (size_t j = 0; j < m->len; j++)
        {
          item = m->items[j];
          if (item->id == id)
            goto found;
        }
    }
  // no match
  uiprivImplBug ("unknown menu ID %hu in runMenuEvent()", id);

found:
  if (item != nullptr)
    {
      if (item->type == typeCheckbox)
        {
          const int checked = uiMenuItemChecked (item);
          uiMenuItemSetChecked (item, checked == 0); // NOLINT(*-implicit-bool-conversion)
        }

      (*item->onClicked) (item, w, item->onClickedData);
    }
}

static void
freeMenu (const uiMenu *m, const HMENU submenu)
{
  size_t j;

  for (size_t i = 0; i < m->len; i++)
    {
      uiMenuItem *item = m->items[i];
      for (j = 0; j < item->len; j++)
        if (item->hmenus[j] == submenu)
          break;
      if (j >= item->len)
        uiprivImplBug ("submenu handle %p not found in freeMenu()", submenu);
      for (; j < item->len - 1; j++)
        item->hmenus[j] = item->hmenus[j + 1];
      item->hmenus[j] = nullptr;
      item->len--;
    }
}

void
freeMenubar (const HMENU menubar)
{
  MENUITEMINFOW mi;

  for (size_t i = 0; i < len; i++)
    {
      ZeroMemory (&mi, sizeof (MENUITEMINFOW));
      mi.cbSize = sizeof (MENUITEMINFOW);
      mi.fMask  = MIIM_SUBMENU;

      if (GetMenuItemInfoW (menubar, i, TRUE, &mi) == 0)
        (void)logLastError (L"error getting menu to delete item references from");

      freeMenu (menus[i], mi.hSubMenu);
    }
}

void
uninitMenus (void)
{

  for (size_t i = 0; i < len; i++)
    {
      uiMenu *m = menus[i];
      uiprivFree (m->name);
      for (size_t j = 0; j < m->len; j++)
        {
          uiMenuItem *item = m->items[j];
          if (item->len != 0)
            uiprivImplBug ("menu item %p (%ws) still has uiWindows attached; did you forget to destroy some windows?",
                           item, item->name);

          if (item->name != nullptr)
            uiprivFree (item->name);

          if (item->hmenus != nullptr)
            uiprivFree (item->hmenus); // NOLINT(*-multi-level-implicit-pointer-conversion)

          uiprivFree (item);
        }
      if (m->items != nullptr)
        uiprivFree (m->items); // NOLINT(*-multi-level-implicit-pointer-conversion)
      uiprivFree (m);
    }
  if (menus != nullptr)
    uiprivFree (menus); // NOLINT(*-multi-level-implicit-pointer-conversion)

  /* Reset global state. */
  menus          = nullptr;
  len            = 0;
  cap            = 0;
  menusFinalized = FALSE;
  curID          = 100;
  hasQuit        = FALSE;
  hasPreferences = FALSE;
  hasAbout       = FALSE;
}
