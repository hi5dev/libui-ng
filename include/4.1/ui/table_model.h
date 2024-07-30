#pragma once

#include "api.h"
#include "table_value.h"

/**
 * @brief Parameter to editable model columns to signify all rows are never editable.
 */
#define uiTableModelColumnNeverEditable (-1)

/**
 * @brief Table model delegate to retrieve data and inform about model changes.
 *
 * This is a wrapper around uiTableModelHandler where the actual data is held.
 *
 * The main purpose it to provide methods to the developer to signal that underlying data has changed.
 *
 * Row indexes match both the row indexes in uiTable and uiTableModelHandler.
 *
 * A uiTableModel can be used as the backing store for multiple uiTable views.
 *
 * Once created, the number of columns and their data types are not allowed to change.
 *
 * @remark Not informing the uiTableModel about out-of-band data changes is an error. User edits via uiTable do *not*
 * fall in this category.
 */
typedef struct uiTableModel uiTableModel;

/**
 * @brief Developer defined methods for data retrieval and setting.
 *
 * These methods get called whenever the associated uiTableModel needs to
 * retrieve data or a uiTable wants to set data.
 *
 * @remark These methods are NOT allowed to change as soon as the uiTableModelHandler is associated with a
 * uiTableModel.
 */
typedef struct uiTableModelHandler uiTableModelHandler;

struct uiTableModelHandler
{
  /**
   * @brief Returns the number of columns in the uiTableModel.
   * @remark This value MUST remain constant throughout the lifetime of the @p uiTableModel.
   * @remark This method is not guaranteed to be called depending on the system.
   */
  int (*NumColumns) (uiTableModelHandler *, uiTableModel *);

  /**
   * @brief Returns the column type in for of a @p uiTableValueType.
   * @remark This value MUST remain constant throughout the lifetime of the @p uiTableModel.
   * @remark This method is not guaranteed to be called depending on the system.
   */
  uiTableValueType (*ColumnType) (uiTableModelHandler *, uiTableModel *, int column);

  /**
   * @brief Returns the number of rows in the @p uiTableModel.
   */
  int (*NumRows) (uiTableModelHandler *, uiTableModel *);

  /**
   * @remark Make sure to use the @p uiTableValue constructors. The returned value must match the @p uiTableValueType
   * defined in @p ColumnType().
   *
   * @remark Some columns may return @p NULL as a special value. Refer to the appropriate @p uiTableAppend*Column()
   * documentation.
   *
   * @remark @p uiTableValue objects are automatically freed when requested by a @p uiTable.
   *
   * @returns the cell value for (row, column).
   */
  uiTableValue *(*CellValue) (uiTableModelHandler *mh, uiTableModel *m, int row, int column);

  /**
   * @brief Sets the cell value for (row, column).
   *
   * It is up to the handler to decide what to do with the value: change the model or reject the change, keeping the
   * old value.
   *
   * Some columns may call this function with `NULL` as a special value. Refer to the appropriate
   * @p uiTableAppend*Column() documentation.
   *
   * @remark @p uiTableValue objects are automatically freed upon return when set by a @p uiTable.
   */
  void (*SetCellValue) (uiTableModelHandler *, uiTableModel *, int, int, const uiTableValue *);
};

/**
 * @brief @p uiTableModel constructor
 * @param mh @p uiTableModelHandler
 * @return @p uiTableModel
 */
API uiTableModel *uiNewTableModel (uiTableModelHandler *mh);

/**
 * @brief @p uiTableModel destructor
 * @param m @p uiTableModel
 * @remark It is an error to free table models currently associated with a @p uiTable.
 */
API void uiFreeTableModel (uiTableModel *m);

/**
 * @brief Informs all associated @p uiTable views that a new row has been added.
 * @param m @p uiTableModel
 * @param newIndex of the row that has been added
 * @remark You must insert the row data in your model before calling this function.
 * @remark @p NumRows must represent the new row count before you call this function.
 */
API void uiTableModelRowInserted (uiTableModel *m, int newIndex);

/**
 * @brief Informs all associated @p uiTable views that a row has been changed.
 * @param m @p uiTableModel
 * @param index of the row that has changed
 * @remark You do not need to call this in your @p SetCellValue handlers, but do need to call this if your data
 * changes at any other point.
 */
API void uiTableModelRowChanged (uiTableModel *m, int index);

/**
 * @brief Informs all associated uiTable views that a row has been deleted.
 * @param m @p uiTableModel
 * @param oldIndex of the row that has been deleted
 * @remark You must delete the row from your model before you call this function.
 * @remark @p NumRows must represent the new row count before you call this function.
 */
API void uiTableModelRowDeleted (uiTableModel *m, int oldIndex);
