#pragma once

#include "image.h"
#include "table_model.h"
#include "table_value.h"

/**
 * @addtogroup table
 * @{
 *
 * Types and methods for organizing and displaying tabular data.
 *
 * Tables follow the concept of separation of concerns, similar to common patterns like model-view-controller or
 * model-view-adapter.
 *
 * They consist of three main components:
 *
 * 1. @p uiTableModel acts as a delegate for the underlying data store. Its purpose is to provide the data for views
 * and inform about any updates.
 * 2. @p uiTable represents the view. Its purpose is to display the data provided by the model as well as provide an
 * interface to the user to modify data.
 * 3. @p uiTableModelHandler takes on the role of controller and model. It provides the actual data while also handling
 * data edits.
 *
 * To get started:
 *
 * 1. Implement all of the methods defined in @p uiTableModelHandler. This involves defining columns, their data
 * types as well as getters and setters for each table cell.
 * 2. Wrap the @p uiTableModelHandler from step 1 in a @p uiTableModel object.
 * 3. Create a new @p uiTable using the model created in step 2.
 * 4. Start adding columns to your table. Each table column is backed by one or more model columns.
 *
 * You can create multiple differing views (@p uiTable) using the same @p uiTableModel.
 * @}
 */

#include "api.h"

/**
 * @brief Type-casts a pointer to a @p uiTable
 * @param this pointer to cast
 */
#define uiTable(this) ((uiTable *)(this))

/**
 * @brief A @p uiControl to display data in a tabular fashion.
 *
 * - Data is retrieved from a @p uiTableModel via methods that you need to define in a @p uiTableModelHandler.
 * - Make sure the @p uiTableModel columns return the right type, as specified by @p uiTableAppend*Column parameters.
 * - The @p *EditableModelColumn parameters typically point to a @p uiTableModel column index, that specifies the
 *   property on a per row basis.
 *
 * They can however also be passed two special values defining the property for all rows:
 * - @p uiTableModelColumnNeverEditable, and
 * - @p uiTableModelColumnAlwaysEditable.
 */
typedef struct uiTable uiTable;

/**
 * @brief @p uiTable constructor parameters
 */
typedef struct uiTableParams uiTableParams;

/**
 * @brief Holds an array of selected row indices for a @p uiTable
 */
typedef struct uiTableSelection uiTableSelection;

/**
 * @brief Optional parameters to control the appearance of text columns.
 */
typedef struct uiTableTextColumnOptionalParams uiTableTextColumnOptionalParams;

/**
 * @brief Table selection that enforce how a user can interact with a table.
 * @remarks An empty table selection is a valid state for any selection mode. This is in fact the default upon table
 * creation and can otherwise triggered through operations such as row deletion.
 * @remarks @p uiTableSelectionModeNone mode disables all editing of text columns. Buttons and checkboxes keep working
 * though.
 */
typedef enum uiTableSelectionMode
{
  uiTableSelectionModeNone,       //!< Allow no row selection.
  uiTableSelectionModeZeroOrOne,  //!< Allow zero or one row to be selected.
  uiTableSelectionModeOne,        //!< Allow for exactly one row to be selected.
  uiTableSelectionModeZeroOrMany, //!< Allow zero or many (multiple) rows to be selected.
} uiTableSelectionMode;

/**
 * @brief Generic sort indicators to display sorting direction.
 */
typedef enum uiSortIndicator
{
  uiSortIndicatorNone,      //!< No sorting.
  uiSortIndicatorAscending, //!< Sort lexigraphically in ascending order.
  uiSortIndicatorDescending //!< Sort lexigraphically in decending order.
} uiSortIndicator;

struct uiTableParams
{
  /**
   * @brief Model holding the data to be displayed.
   * @remark This can NOT be @p NULL.
   */
  uiTableModel *Model;

  /**
   * @brief @p uiTableModel column that defines background color for each row,
   * @li @p uiTableValueTypeColor background color,
   * @li @p NULL to use the default background color for that row,
   * @li @p -1 to use the default background color for all rows.
   */
  int RowBackgroundColorModelColumn;
};

struct uiTableSelection
{
  int  NumRows; //!< Number of selected rows.
  int *Rows;    //!< Array containing selected row indices, NULL on empty selection.
};

struct uiTableTextColumnOptionalParams
{
  /**
   * @brief @p uiTableModel column that defines the text color for each cell.
   *
   * @li @p uiTableValueTypeColor Text color, @p NULL to use the default color for that cell.
   * @li @p -1 to use the default color for all cells.
   */
  int ColorModelColumn;
};

/**
 * @brief Callback for @p uiTable header click events
 * @param table reference to the instance that triggered the callback
 * @param index of the row or column that was clicked
 * @param data registered with the sender instance
 */
typedef void (uiTableHeaderCallback) (uiTable *table, int index, void *data);

/**
 * @brief Callback for @p uiTable selection change events
 * @param sender reference to the instance that triggered the callback
 * @param senderData user-data registered with the sender instance
 */
typedef void (uiTableSelectionChangeCallback) (uiTable *sender, void *senderData);

/**
 * @brief Appends a text column to a @p uiTable
 * @param t @p uiTable
 * @param name string
 * @param textModelColumn @p uiTableValueTypeString column that holds the text to be displayed.
 * @param textEditableModelColumn Determines if data in the column can be edited:
 *   - @p uiTableValueTypeInt with a non-zero value for editable text
 *   - @p uiTableModelColumnNeverEditable to make all rows never editable
 *   - @p uiTableModelColumnAlwaysEditable to make all rows always editable
 * @param textParams text display settings or @p NULL for defaults
 * @remark @p name is copied internally; ownership is not transferred
 */
API void uiTableAppendTextColumn (uiTable *t, const char *name, int textModelColumn, int textEditableModelColumn,
                                  uiTableTextColumnOptionalParams *textParams);

/**
 * @brief Appends an image column to a @p uiTable
 * @param t uiTable instance.
 * @param name string
 * @param imageModelColumn @p uiTableValueTypeImage column that holds the images to be displayed
 * @remark Images are drawn at icon size, using the representation that best fits the pixel density of the screen.
 * @remark @p name is copied internally; ownership is not transferred
 */
API void uiTableAppendImageColumn (uiTable *t, const char *name, int imageModelColumn);

/**
 * @brief Appends a column to a @p uiTable that displays both an image and text.
 * @param t @p uiTable
 * @param name string
 * @param imageModelColumn @p uiTableValueTypeImage column that holds the images to be displayed
 * @param textModelColumn @p uiTableValueTypeString column that holds the text to be displayed.
 * @param textEditableModelColumn A column of one of these types to define whether or not the text is editable:
 *  - @p uiTableValueTypeInt with a non-zero value for editable
 *  - @p uiTableModelColumnNeverEditable to make all rows never editable
 *  - @p uiTableModelColumnAlwaysEditable to make all rows always editable
 * @param textParams text display settings or @p NULL for defaults
 * @remark Images are drawn at icon size, using the representation that best fits the pixel density of the screen.
 * @remark @p name is copied internally; ownership is not transferred
 */
API void uiTableAppendImageTextColumn (uiTable *t, const char *name, int imageModelColumn, int textModelColumn,
                                       int textEditableModelColumn, uiTableTextColumnOptionalParams *textParams);

/**
 * @brief Appends a column to a @p uiTable containing a checkbox
 * @param t @p uiTable
 * @param name string
 * @param checkboxModelColumn @p uiTableValueTypeInt column that holds the data to be displayed
 * @param checkboxEditableModelColumn A column of one of these types to define whether or not the checkbox is editable:
 *  - @p uiTableValueTypeInt with a non-zero value for editable
 *  - @p uiTableModelColumnNeverEditable to make all rows never editable
 *  - @p uiTableModelColumnAlwaysEditable to make all rows always editable
 * @remark @p name is copied internally; ownership is not transferred
 */
API void uiTableAppendCheckboxColumn (uiTable *t, const char *name, int checkboxModelColumn,
                                      int checkboxEditableModelColumn);

/**
 * @brief Appends a column to a @p uiTable containing a checkbox and text
 * @param t uiTable instance.
 * @param name string
 * @param checkboxModelColumn @p uiTableValueTypeInt column that holds the data to be displayed
 * @param checkboxEditableModelColumn A column of one of these types to define whether or not the checkbox is editable:
 *  - @p uiTableValueTypeInt with a non-zero value for editable
 *  - @p uiTableModelColumnNeverEditable to make all rows never editable
 *  - @p uiTableModelColumnAlwaysEditable to make all rows always editable
 * @param textModelColumn @p uiTableValueTypeString column that holds the text to be displayed
 * @param textEditableModelColumn A column of one of these types to define whether or not the text is editable:
 *  - @p uiTableValueTypeInt with a non-zero value for editable
 *  - @p uiTableModelColumnNeverEditable to make all rows never editable
 *  - @p uiTableModelColumnAlwaysEditable to make all rows always editable
 * @param textParams Text display settings, `NULL` to use defaults.
 * @remark @p name is copied internally; ownership is not transferred
 */
API void uiTableAppendCheckboxTextColumn (uiTable *t, const char *name, int checkboxModelColumn,
                                          int checkboxEditableModelColumn, int textModelColumn,
                                          int textEditableModelColumn, uiTableTextColumnOptionalParams *textParams);

/**
 * @brief Appends a column to a @p uiTable containing a progress bar
 * @param t @p uiTable
 * @param name string
 * @param progressModelColumn @p uiTableValueTypeInt column that holds the data to be displayed
 * @remark @p name is copied internally; ownership is not transferred
 * @see uiProgressBar
 */
API void uiTableAppendProgressBarColumn (uiTable *t, const char *name, int progressModelColumn);

/**
 * @brief Appends a column to a @p uiTable containing a button
 *
 * Button clicks are signaled to the uiTableModelHandler via a call to @p SetCellValue with a value of @p NULL for
 * the @p buttonModelColumn.

 * @param t uiTable instance.
 * @param name string
 * @param buttonModelColumn @p uiTableValueTypeString column that holds the button text to be displayed
 * @param buttonClickableModelColumn A column of one of these types to determine if the button can be clicked:
 *  - @p uiTableValueTypeInt with a non-zero value for clickable
 *  - @p uiTableModelColumnNeverEditable to make all rows never clickable
 *  - @p uiTableModelColumnAlwaysEditable to make all rows always clickable
 * @remark @p name is copied internally; ownership is not transferred
 * @remark @p CellValue must return the button text to display
 */
API void uiTableAppendButtonColumn (uiTable *t, const char *name, int buttonModelColumn,
                                    int buttonClickableModelColumn);

/**
 * @brief Checks if a @p uiTable header is visible.
 * @param t @p uiTable
 * @return non-zero when true
 */
API int uiTableHeaderVisible (uiTable *t);

/**
 * @brief Sets the visibility of a @p uiTable header
 * @param t @p uiTable
 * @param visible non-zero to show, zero to hide
 */
API void uiTableHeaderSetVisible (uiTable *t, int visible);

/**
 * @brief @p uiTable constructor
 * @param params @p uiTableParams
 * @return @p uiTable
 */
API uiTable *uiNewTable (uiTableParams *params);

/**
 * @brief Registers a callback for when the user single clicks a table row.
 * @param t @p uiTable
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark only one callback can be registered at a time
 */
API void uiTableOnRowClicked (uiTable *t, uiTableHeaderCallback *f, void *data);

/**
 * @brief Registers a callback for when the user double clicks a table row.
 * @param t @p uiTable
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark the double click callback is always preceded by one @p uiTableOnRowClicked callback
 * @remark only one callback can be registered at a time
 */
API void uiTableOnRowDoubleClicked (uiTable *t, uiTableHeaderCallback *f, void *data);

/**
 * @brief Sets a column's sort indicator displayed in the @p uiTable header
 * @param t @p uiTable
 * @param column index
 * @param indicator @p uiSortIndicator
 * @remark the indicator is purely visual and does not perform any actual sorting
 */
API void uiTableHeaderSetSortIndicator (uiTable *t, int column, uiSortIndicator indicator);

/**
 * @brief Gets a column's sort indicator displayed in a @p uiTable header
 * @param t @p uiTable
 * @param column index
 * @return @p uiSortIndicator
 */
API uiSortIndicator uiTableHeaderSortIndicator (uiTable *t, int column);

/**
 * @brief Registers a callback for when a @p uiTable column header is clicked
 * @param t @p uiTable
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark Only one callback can be registered at a time.
 */
API void uiTableHeaderOnClicked (uiTable *t, uiTableHeaderCallback *f, void *data);

/**
 * @brief Gets the width of a @p uiTable column
 * @param t @p uiTable
 * @param column index
 * @return width in pixels
 */
API int uiTableColumnWidth (uiTable *t, int column);

/**
 * @brief Sets the width of a @p uiTable column
 * @param t @p uiTable
 * @param column index
 * @param width in pixels or  @p -1 to restore automatic column sizing
 */
API void uiTableColumnSetWidth (uiTable *t, int column, int width);

/**
 * @brief Gets the selection mode of a @p uiTable
 * @param t @p uiTable
 * @return @p uiTableSelectionMode
 */
API uiTableSelectionMode uiTableGetSelectionMode (uiTable *t);

/**
 * @brief Sets the selection mode of a @p uiTable
 * @param t @p uiTable
 * @param mode @p uiTableSelectionMode
 * @remark all rows will be deselected if the existing selection is illegal in the new selection mode
 */
API void uiTableSetSelectionMode (uiTable *t, uiTableSelectionMode mode);

/**
 * @brief Registers a callback for when the table selection changed.
 * @param t uiTable instance.
 * @param f pointer to the callback function
 * @param data to be passed to the callback
 * @remark the callback is not triggered when
 *         - calling @p uiTableSetSelection, or
 *         - if the selection should be cleared for @p uiTableSetSelectionMode
 * @remark only one callback can be registered at a time
 */
API void uiTableOnSelectionChanged (uiTable *t, uiTableSelectionChangeCallback *f, void *data);

/**
 * @brief Gets the current selection of a @p uiTable
 * @param t @p uiTable
 * @return number of selected rows and corresponding row indices
 * @remark for empty selections the @p Rows pointer will be @p NULL
 * @remark caller is responsible for freeing the data
 * @see uiFreeTableSelection
 */
API uiTableSelection *uiTableGetSelection (uiTable *t);

/**
 * @brief Clears and sets the current selection of a @p uiTable
 * @param t @p uiTable
 * @param sel @p uiTableSelection
 * @remark @p sel is copied internally; ownership is not transferred
 * @remark nothing happens when selecting more rows than the selection mode allows
 * @remark @p Rows pointer is never accessed for empty selections
 */
API void uiTableSetSelection (uiTable *t, uiTableSelection *sel);

/**
 * @brief @p uiTableSelection destructor
 * @param s @p uiTableSelection
 */
API void uiFreeTableSelection (uiTableSelection *s);
