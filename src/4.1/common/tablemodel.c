#include <ui/table.h>
#include <ui/table_model.h>

#include "uipriv.h"

#include <ui/tab.h>

int
uiprivTableModelNumColumns (uiTableModel *m)
{
  uiTableModelHandler *mh = uiprivTableModelHandler (m);
  return (*mh->NumColumns) (mh, m);
}

uiTableValueType
uiprivTableModelColumnType (uiTableModel *m, const int column)
{
  uiTableModelHandler *mh = uiprivTableModelHandler (m);
  return (*mh->ColumnType) (mh, m, column);
}

int
uiprivTableModelNumRows (uiTableModel *m)
{
  uiTableModelHandler *mh = uiprivTableModelHandler (m);
  return (*mh->NumRows) (mh, m);
}

uiTableValue *
uiprivTableModelCellValue (uiTableModel *m, const int row, const int column)
{

  uiTableModelHandler *mh = uiprivTableModelHandler (m);
  return (*mh->CellValue) (mh, m, row, column);
}

void
uiprivTableModelSetCellValue (uiTableModel *m, const int row, const int column, const uiTableValue *value)
{

  uiTableModelHandler *mh = uiprivTableModelHandler (m);
                       (*mh->SetCellValue) (mh, m, row, column, value);

  uiTableModelRowChanged (m, row);
}

const uiTableTextColumnOptionalParams uiprivDefaultTextColumnOptionalParams = {
  .ColorModelColumn = -1,
};

int
uiprivTableModelCellEditable (uiTableModel *m, const int row, const int column)
{
  switch (column)
    {
    case uiTableModelColumnNeverEditable:
      return 0;

    case uiTableModelColumnAlwaysEditable:
      return 1;

    default:;
      break;
    }

  uiTableValue *value    = uiprivTableModelCellValue (m, row, column);
  const int     editable = uiTableValueInt (value);

  uiFreeTableValue (value);

  return editable;
}

int
uiprivTableModelColorIfProvided (uiTableModel *m, const int row, const int column, double *r, double *g, double *b,
                                 double *a)
{

  if (column == -1)
    return 0;

  uiTableValue *value = uiprivTableModelCellValue (m, row, column);

  if (value == NULL)
    return 0;

  uiTableValueColor (value, r, g, b, a);

  uiFreeTableValue (value);

  return 1;
}
