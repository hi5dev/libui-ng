#pragma once

#include <ui/table.h>
#include <ui/table_model.h>

#include <commctrl.h>
#include <ui_win32.h>

#include <map>

#define uiprivNumLVN_GETDISPINFOSkip 3

typedef struct uiprivTableColumnParams uiprivTableColumnParams;

struct uiTable
{
  uiWindowsControl c;

  uiTableModel *model;

  HWND hwnd;

  std::vector<uiprivTableColumnParams *> *columns;

  WPARAM nColumns;

  int backgroundColumn;

  HIMAGELIST imagelist;

  std::map<std::pair<int, int>, LONG> *indeterminatePositions;

  BOOL inLButtonDown;

  BOOL inDoubleClickTimer;

  HWND edit;

  int editedItem;

  int editedSubitem;

  uiTableSelectionMode selectionMode;

  BOOL maskOnSelectionChanged;

  // Cache last focused item to signal selection changes
  int lastFocusedItem;

  // Cache if last focused item is selected to signal selection changes
  BOOL lastFocusedItemIsSelected;

  // Cache last number of selected items to signal selection changes
  int lastNumSelected;

  void (*headerOnClicked) (uiTable *, int, void *);

  void *headerOnClickedData;

  void (*onRowClicked) (uiTable *, int, void *);

  void *onRowClickedData;

  void (*onRowDoubleClicked) (uiTable *, int, void *);

  void *onRowDoubleClickedData;

  void (*onSelectionChanged) (uiTable *, void *);

  void *onSelectionChangedData;
};

struct uiprivTableColumnParams
{
  int textModelColumn;

  int textEditableModelColumn;

  uiTableTextColumnOptionalParams textParams;

  int imageModelColumn;

  int checkboxModelColumn;

  int checkboxEditableModelColumn;

  int progressBarModelColumn;

  int buttonModelColumn;

  int buttonClickableModelColumn;
};

extern int uiprivTableProgress (uiTable *t, int item, int subitem, int modelColumn, LONG *pos);
