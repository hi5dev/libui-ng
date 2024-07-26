#pragma once

#include "ui/area.h"
#include "ui/box.h"
#include "ui/combobox.h"
#include "ui/form.h"
#include "ui/grid.h"
#include "ui/group.h"
#include "ui/image.h"
#include "ui/menu_item.h"
#include "ui/tab.h"

// main.c
extern void die(const char *, ...);
extern uiBox *mainBox;
extern uiTab *mainTab;
extern uiBox *(*newhbox)(void);
extern uiBox *(*newvbox)(void);

// spaced.c
extern void setSpaced(int);
extern void querySpaced(char[12]);
extern uiWindow *newWindow(const char *title, int width, int height, int hasMenubar);
extern uiBox *newHorizontalBox(void);
extern uiBox *newVerticalBox(void);
extern uiTab *newTab(void);
extern uiGroup *newGroup(const char *);
extern uiForm *newForm(void);
extern uiGrid *newGrid(void);

// menus.c
extern uiMenuItem *shouldQuitItem;
extern void initMenus(void);

// page1.c
extern uiBox *page1;
extern void makePage1(uiWindow *);

// page2.c
extern uiGroup *page2group;
extern uiBox *makePage2(void);

// page3.c
extern uiBox *makePage3(void);

// page4.c
extern uiBox *makePage4(void);

// page5.c
extern uiBox *makePage5(uiWindow *);

// page6.c
extern uiBox *makePage6(void);

// drawtests.c
extern void runDrawTest(int, uiAreaDrawParams *);
extern void populateComboboxWithTests(uiCombobox *);

// page7.c
extern uiBox *makePage7(void);

// page7a.c
extern uiGroup *makePage7a(void);

// page7b.c
extern uiGroup *makePage7b(void);

// page7c.c
extern uiGroup *makePage7c(void);

// page8.c
extern uiBox *makePage8(void);

// page9.c
extern uiBox *makePage9(void);

// page10.c
extern uiBox *makePage10(void);

// page11.c
extern uiBox *makePage11(void);

// page12.c
extern uiBox *makePage12(void);

// page13.c
extern uiBox *makePage13(void);

// page14.c
extern uiTab *makePage14(void);

// page15.c
extern uiBox *makePage15(uiWindow *);

// page16.c
extern uiBox *makePage16(void);
extern void freePage16(void);

// page17.c
extern uiBox *makePage17(void);
extern void freePage17(void);

// images.c
extern void appendImageNamed(uiImage *img, const char *name);
