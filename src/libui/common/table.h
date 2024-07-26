#pragma once

#include "image.h"

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
 * @brief Parameter to editable model columns to signify all rows are never editable.
 */
#define uiTableModelColumnNeverEditable (-1)

/**
 * @brief A control to display data in a tabular fashion.
 *
 * The view of the architecture.
 *
 * Data is retrieved from a uiTableModel via methods that you need to define in a uiTableModelHandler.
 *
 * Make sure the uiTableModel columns return the right type, as specified in the @p uiTableAppend*Column() parameters.
 *
 * The @p *EditableModelColumn parameters typically point to a uiTableModel column index, that specifies the property
 * on a per row basis.
 *
 * They can however also be passed two special values defining the property for all rows: @p
 * uiTableModelColumnNeverEditable and @p uiTableModelColumnAlwaysEditable.
 */
typedef struct uiTable uiTable;

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

/**
 * @brief Table parameters passed to uiNewTable().
 */
typedef struct uiTableParams uiTableParams;

/**
 * @brief Holds an array of selected row indices for a table.
 */
typedef struct uiTableSelection uiTableSelection;

/**
 * @brief Optional parameters to control the appearance of text columns.
 */
typedef struct uiTableTextColumnOptionalParams uiTableTextColumnOptionalParams;

/**
 * @brief Container to store values used in container related methods.
 * @remark @p uiTableValue objects are immutable.
 * @remark @p uiTable and @p uiTableModel methods take ownership of the @p uiTableValue objects when passed as
 * parameter. Exception: @p uiNewTableValueImage().
 * @remark @p uiTable and @p uiTableModel methods retain ownership when returning @p uiTableValue objects. Exception:
 * @p uiTableValueImage().
 */
typedef struct uiTableValue uiTableValue;

/**
 * @brief Table selection that enforce how a user can interact with a table.
 * @remarks An empty table selection is a valid state for any selection mode. This is in fact the default upon table
 * creation and can otherwise triggered through operations such as row deletion.
 * @remarks @p uiTableSelectionModeNone mode disables all editing of text columns. Buttons and checkboxes keep working
 * though.
 */
typedef enum uiTableSelectionMode : int8_t
{
  uiTableSelectionModeNone,       //!< Allow no row selection.
  uiTableSelectionModeZeroOrOne,  //!< Allow zero or one row to be selected.
  uiTableSelectionModeOne,        //!< Allow for exactly one row to be selected.
  uiTableSelectionModeZeroOrMany, //!< Allow zero or many (multiple) rows to be selected.
} uiTableSelectionMode;

/**
 * @brief @p uiTableValue types.
 */
typedef enum uiTableValueType : int8_t
{
  uiTableValueTypeString,
  uiTableValueTypeImage,
  uiTableValueTypeInt,
  uiTableValueTypeColor,
} uiTableValueType;

/**
 * @brief Generic sort indicators to display sorting direction.
 */
typedef enum uiSortIndicator : int8_t
{
  uiSortIndicatorNone,      //!< No sorting.
  uiSortIndicatorAscending, //!< Sort lexigraphically in ascending order.
  uiSortIndicatorDescending //!< Sort lexigraphically in decending order.
} uiSortIndicator;

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

API uiTableModelHandler *uiprivTableModelHandler (uiTableModel *m);

API uiTableValue *uiprivTableModelCellValue (uiTableModel *m, int row, int column);

API uiTableValueType uiprivTableModelColumnType (uiTableModel *m, int column);

API int uiprivTableModelCellEditable (uiTableModel *m, int row, int column);

API int uiprivTableModelColorIfProvided (uiTableModel *m, int row, int column, double *r, double *g, double *b,
                                         double *a);

API int uiprivTableModelNumColumns (uiTableModel *m);

API int uiprivTableModelNumRows (uiTableModel *m);

API void uiprivTableModelSetCellValue (uiTableModel *m, int row, int column, const uiTableValue *value);

/**
 * Frees the uiTableValue.
 *
 * @param v Table value to free.
 *
 * @warning This function is to be used only on uiTableValue objects that
 *          have NOT been passed to uiTable or uiTableModel - as these
 *          take ownership of the object.\n
 *          Use this for freeing erroneously created values or when directly
 *          calling uiTableModelHandler without transferring ownership to
 *          uiTable or uiTableModel.
 * @memberof uiTableValue
 */
API void uiFreeTableValue (uiTableValue *v);

/**
 * Gets the uiTableValue type.
 *
 * @param v Table value.
 * @returns Table value type.
 * @memberof uiTableValue
 */
API uiTableValueType uiTableValueGetType (const uiTableValue *v);

/**
 * Creates a new table value to store a text string.
 *
 * @param str String value.\n
 *            A valid, `NUL` terminated UTF-8 string.\n
 *            Data is copied internally. Ownership is not transferred.
 * @returns A new uiTableValue instance.
 * @memberof uiTableValue @static
 */
API uiTableValue *uiNewTableValueString (const char *str);

/**
 * Returns the string value held internally.
 *
 * To be used only on uiTableValue objects of type uiTableValueTypeString.
 *
 * @param v Table value.
 * @returns String value.\n
 *          A `NUL` terminated UTF-8 string.\n
 *          Data remains owned by @p v, do **NOT** call `uiFreeText()`.
 * @memberof uiTableValue
 */
API const char *uiTableValueString (const uiTableValue *v);

/**
 * Creates a new table value to store an image.
 *
 * @param img Image.\n
 *            Data is NOT copied and needs to kept alive.
 * @returns A new uiTableValue instance.
 * @warning Unlike other uiTableValue constructors, uiNewTableValueImage() does
 *          NOT copy the image to save time and space. Make sure the image
 *          data stays valid while in use by the library.
 *          As a general rule: if the constructor is called via the
 *          uiTableModelHandler, the image is safe to free once execution
 *          returns to ANY of your code.
 * @memberof uiTableValue @static
 */
API uiTableValue *uiNewTableValueImage (uiImage *img);

/**
 * Returns a reference to the image contained.
 *
 * To be used only on uiTableValue objects of type uiTableValueTypeImage.
 *
 * @param v Table value.
 * @returns Image.\n
 *          Data is owned by the caller of uiNewTableValueImage().
 * @warning The image returned is not owned by the object @p v,
 *          hence no lifetime guarantees can be made.
 * @memberof uiTableValue
 */
API uiImage *uiTableValueImage (const uiTableValue *v);

/**
 * Creates a new table value to store an integer.
 *
 * This value type can be used in conjunction with properties like
 * column editable [`TRUE`, `FALSE`] or controls like progress bars and
 * checkboxes. For these, consult uiProgressBar and uiCheckbox for the allowed
 * integer ranges.
 *
 * @param i Integer value.
 * @returns A new uiTableValue instance.
 * @memberof uiTableValue @static
 */
API uiTableValue *uiNewTableValueInt (int i);

/**
 * Returns the integer value held internally.
 *
 * To be used only on uiTableValue objects of type uiTableValueTypeInt.
 *
 * @param v Table value.
 * @returns Integer value.
 * @memberof uiTableValue
 */
API int uiTableValueInt (const uiTableValue *v);

/**
 * Creates a new table value to store a color in.
 *
 * @param r Red. Double in range of [0, 1.0].
 * @param g Green. Double in range of [0, 1.0].
 * @param b Blue. Double in range of [0, 1.0].
 * @param a Alpha. Double in range of [0, 1.0].
 * @returns A new uiTableValue instance.
 * @memberof uiTableValue @static
 */
API uiTableValue *uiNewTableValueColor (double r, double g, double b, double a);

/**
 * Returns the color value held internally.
 *
 * To be used only on uiTableValue objects of type uiTableValueTypeColor.
 *
 * @param v Table value.
 * @param[out] r Red. Double in range of [0, 1.0].
 * @param[out] g Green. Double in range of [0, 1.0].
 * @param[out] b Blue. Double in range of [0, 1.0].
 * @param[out] a Alpha. Double in range of [0, 1.0].
 * @memberof uiTableValue
 */
API void uiTableValueColor (const uiTableValue *v, double *r, double *g, double *b, double *a);

/**
 * Creates a new table model.
 *
 * @param mh Table model handler.
 * @returns A new uiTableModel instance.
 * @memberof uiTableModel @static
 */
API uiTableModel *uiNewTableModel (uiTableModelHandler *mh);

/**
 * Frees the table model.
 *
 * @param m Table model to free.
 * @warning It is an error to free table models currently associated with a
 *          uiTable.
 * @memberof uiTableModel
 */
API void uiFreeTableModel (uiTableModel *m);

/**
 * Informs all associated uiTable views that a new row has been added.
 *
 * You must insert the row data in your model before calling this function.
 *
 * NumRows() must represent the new row count before you call this function.
 *
 * @param m Table model that has changed.
 * @param newIndex Index of the row that has been added.
 * @memberof uiTableModel
 */
API void uiTableModelRowInserted (uiTableModel *m, int newIndex);

/**
 * Informs all associated uiTable views that a row has been changed.
 *
 * You do NOT need to call this in your SetCellValue() handlers, but NEED
 * to call this if your data changes at any other point.
 *
 * @param m Table model that has changed.
 * @param index Index of the row that has changed.
 * @memberof uiTableModel
 */
API void uiTableModelRowChanged (uiTableModel *m, int index);

/**
 * Informs all associated uiTable views that a row has been deleted.
 *
 * You must delete the row from your model before you call this function.
 *
 * NumRows() must represent the new row count before you call this function.
 *
 * @param m Table model that has changed.
 * @param oldIndex Index of the row that has been deleted.
 * @memberof uiTableModel
 */
API void uiTableModelRowDeleted (uiTableModel *m, int oldIndex);

/**
 * Appends a text column to the table.
 *
 * @param t uiTable instance.
 * @param name Column title text.\n
 *             A valid, `NUL` terminated UTF-8 string.\n
 *             Data is copied internally. Ownership is not transferred.
 * @param textModelColumn Column that holds the text to be displayed.\n
 *                        #uiTableValueTypeString
 * @param textEditableModelColumn Column that defines whether or not the text is editable.\n
 *                                #uiTableValueTypeInt `TRUE` to make text editable, `FALSE`
 *                                otherwise.\n
 *                                `uiTableModelColumnNeverEditable` to make all rows never editable.\n
 *                                `uiTableModelColumnAlwaysEditable` to make all rows always editable.
 * @param textParams Text display settings, `NULL` to use defaults.
 * @memberof uiTable
 */
API void uiTableAppendTextColumn (uiTable *t, const char *name, int textModelColumn, int textEditableModelColumn,
                                  uiTableTextColumnOptionalParams *textParams);

/**
 * Appends an image column to the table.
 *
 * Images are drawn at icon size, using the representation that best fits the
 * pixel density of the screen.
 *
 * @param t uiTable instance.
 * @param name Column title text.\n
 *             A valid, `NUL` terminated UTF-8 string.\n
 *             Data is copied internally. Ownership is not transferred.
 * @param imageModelColumn Column that holds the images to be displayed.\n
 *                         #uiTableValueTypeImage
 * @memberof uiTable
 */
API void uiTableAppendImageColumn (uiTable *t, const char *name, int imageModelColumn);

/**
 * Appends a column to the table that displays both an image and text.
 *
 * Images are drawn at icon size, using the representation that best fits the
 * pixel density of the screen.
 *
 * @param t uiTable instance.
 * @param name Column title text.\n
 *             A valid, `NUL` terminated UTF-8 string.\n
 *             Data is copied internally. Ownership is not transferred.
 * @param imageModelColumn Column that holds the images to be displayed.\n
 *                         #uiTableValueTypeImage
 * @param textModelColumn Column that holds the text to be displayed.\n
 *                        #uiTableValueTypeString
 * @param textEditableModelColumn Column that defines whether or not the text is editable.\n
 *                                #uiTableValueTypeInt `TRUE` to make text editable, `FALSE` otherwise.\n
 *                                `uiTableModelColumnNeverEditable` to make all rows never editable.\n
 *                                `uiTableModelColumnAlwaysEditable` to make all rows always editable.
 * @param textParams Text display settings, `NULL` to use defaults.
 * @memberof uiTable
 */
API void uiTableAppendImageTextColumn (uiTable *t, const char *name, int imageModelColumn, int textModelColumn,
                                       int textEditableModelColumn, uiTableTextColumnOptionalParams *textParams);

/**
 * Appends a column to the table containing a checkbox.
 *
 * @param t uiTable instance.
 * @param name Column title text.\n
 *             A valid, `NUL` terminated UTF-8 string.\n
 *             Data is copied internally. Ownership is not transferred.
 * @param checkboxModelColumn Column that holds the data to be displayed.\n
 *                            #uiTableValueTypeInt `TRUE` for a checked checkbox, `FALSE` otherwise.
 * @param checkboxEditableModelColumn Column that defines whether or not the checkbox is editable.\n
 *                                    #uiTableValueTypeInt `TRUE` to make checkbox editable, `FALSE` otherwise.\n
 *                                    `uiTableModelColumnNeverEditable` to make all rows never editable.\n
 *                                    `uiTableModelColumnAlwaysEditable` to make all rows always editable.
 * @memberof uiTable
 */
API void uiTableAppendCheckboxColumn (uiTable *t, const char *name, int checkboxModelColumn,
                                      int checkboxEditableModelColumn);

/**
 * Appends a column to the table containing a checkbox and text.
 *
 * @param t uiTable instance.
 * @param name Column title text.\n
 *             A valid, `NUL` terminated UTF-8 string.\n
 *             Data is copied internally. Ownership is not transferred.
 * @param checkboxModelColumn Column that holds the data to be displayed.\n
 *                            #uiTableValueTypeInt
 *                            `TRUE` for a checked checkbox, `FALSE` otherwise.
 * @param checkboxEditableModelColumn Column that defines whether or not the checkbox is editable.\n
 *                                    #uiTableValueTypeInt `TRUE` to make checkbox editable, `FALSE` otherwise.\n
 *                                    `uiTableModelColumnNeverEditable` to make all rows never editable.\n
 *                                    `uiTableModelColumnAlwaysEditable` to make all rows always editable.
 * @param textModelColumn Column that holds the text to be displayed.\n
 *                        #uiTableValueTypeString
 * @param textEditableModelColumn Column that defines whether or not the text is editable.\n
 *                                #uiTableValueTypeInt `TRUE` to make text editable, `FALSE` otherwise.\n
 *                                `uiTableModelColumnNeverEditable` to make all rows never editable.\n
 *                                `uiTableModelColumnAlwaysEditable` to make all rows always editable.
 * @param textParams Text display settings, `NULL` to use defaults.
 * @memberof uiTable
 */
API void uiTableAppendCheckboxTextColumn (uiTable *t, const char *name, int checkboxModelColumn,
                                          int checkboxEditableModelColumn, int textModelColumn,
                                          int textEditableModelColumn, uiTableTextColumnOptionalParams *textParams);

/**
 * Appends a column to the table containing a progress bar.
 *
 * The workings and valid range are exactly the same as that of uiProgressBar.
 *
 * @param t uiTable instance.
 * @param name Column title text.\n
 *             A valid, `NUL` terminated UTF-8 string.\n
 *             Data is copied internally. Ownership is not transferred.
 * @param progressModelColumn Column that holds the data to be displayed.\n
 *                            #uiTableValueTypeInt Integer in range of `[-1, 100]`, see uiProgressBar
 *                            for details.
 * @see uiProgressBar
 * @memberof uiTable
 */
API void uiTableAppendProgressBarColumn (uiTable *t, const char *name, int progressModelColumn);

/**
 * Appends a column to the table containing a button.
 *
 * Button clicks are signaled to the uiTableModelHandler via a call to
 * SetCellValue() with a value of `NULL` for the @p buttonModelColumn.
 *
 * CellValue() must return the button text to display.
 *
 * @param t uiTable instance.
 * @param name Column title text.\n
 *             A valid, `NUL` terminated UTF-8 string.\n
 *             Data is copied internally. Ownership is not transferred.
 * @param buttonModelColumn Column that holds the button text to be displayed.\n
 *                          #uiTableValueTypeString
 * @param buttonClickableModelColumn Column that defines whether or not the button is clickable.\n
 *                                   #uiTableValueTypeInt `TRUE` to make button clickable, `FALSE` otherwise.\n
 *                                   `uiTableModelColumnNeverEditable` to make all rows never clickable.\n
 *                                   `uiTableModelColumnAlwaysEditable` to make all rows always clickable.
 * @memberof uiTable
 */
API void uiTableAppendButtonColumn (uiTable *t, const char *name, int buttonModelColumn,
                                    int buttonClickableModelColumn);

/**
 * Returns whether or not the table header is visible.
 *
 * @param t uiTable instance.
 * @returns `TRUE` if visible, `FALSE` otherwise. [Default `TRUE`]
 * @memberof uiTable
 */
API int uiTableHeaderVisible (uiTable *t);

/**
 * Sets whether or not the table header is visible.
 *
 * @param t uiTable instance.
 * @param visible `TRUE` to show header, `FALSE` to hide header.
 * @memberof uiTable
 */
API void uiTableHeaderSetVisible (uiTable *t, int visible);

/**
 * Creates a new table.
 *
 * @param params Table parameters.
 * @returns A new uiTable instance.
 * @memberof uiTable @static
 */
API uiTable *uiNewTable (uiTableParams *params);

/**
 * Registers a callback for when the user single clicks a table row.
 *
 * @param t uiTable instance.
 * @param f Callback function.\n
 *          @p sender Back reference to the instance that triggered the callback.\n
 *          @p row Row index that was clicked.\n
 *          @p senderData User data registered with the sender instance.
 * @param data User data to be passed to the callback.
 *
 * @note Only one callback can be registered at a time.
 * @memberof uiTable
 */
API void uiTableOnRowClicked (uiTable *t, void (*f) (uiTable *table, int row, void *data), void *data);

/**
 * Registers a callback for when the user double clicks a table row.
 *
 * @param t uiTable instance.
 * @param f Callback function.\n
 *          @p sender Back reference to the instance that triggered the callback.\n
 *          @p row Row index that was double clicked.\n
 *          @p senderData User data registered with the sender instance.
 * @param data User data to be passed to the callback.
 *
 * @note The double click callback is always preceded by one uiTableOnRowClicked() callback.
 * @bug For unix systems linking against `GTK < 3.14` the preceding uiTableOnRowClicked()
 *      callback will be triggered twice.
 * @note Only one callback can be registered at a time.
 * @memberof uiTable
 */
API void uiTableOnRowDoubleClicked (uiTable *t, void (*f) (uiTable *table, int row, void *data), void *data);

/**
 * Sets the column's sort indicator displayed in the table header.
 *
 * Use this to display appropriate arrows in the table header to indicate a
 * sort direction.
 *
 * @param t uiTable instance.
 * @param column Column index.
 * @param indicator Sort indicator.
 * @note Setting the indicator is purely visual and does not perform any sorting.
 * @memberof uiTable
 */
API void uiTableHeaderSetSortIndicator (uiTable *t, int column, uiSortIndicator indicator);

/**
 * Returns the column's sort indicator displayed in the table header.
 *
 * @param t uiTable instance.
 * @param column Column index.
 * @returns The current sort indicator. [Default: `uiSortIndicatorNone`]
 * @memberof uiTable
 */
API uiSortIndicator uiTableHeaderSortIndicator (uiTable *t, int column);

/**
 * Registers a callback for when a table column header is clicked.
 *
 * @param t uiTable instance.
 * @param f Callback function.\n
 *          @p sender Back reference to the instance that triggered the callback.\n
 *          @p column Column index that was clicked.\n
 *          @p senderData User data registered with the sender instance.
 * @param data User data to be passed to the callback.
 *
 * @note Only one callback can be registered at a time.
 * @memberof uiTable
 */
API void uiTableHeaderOnClicked (uiTable *t, void (*f) (uiTable *sender, int column, void *senderData), void *data);

/**
 * Returns the table column width.
 *
 * @param t uiTable instance.
 * @param column Column index.
 * @returns Column width in pixels.
 * @memberof uiTable
 */
API int uiTableColumnWidth (uiTable *t, int column);

/**
 * Sets the table column width.
 *
 * Setting the width to `-1` will restore automatic column sizing matching
 * either the width of the content or column header (which ever one is bigger).
 * @note Darwin currently only resizes to the column header width on `-1`.
 *
 * @param t uiTable instance.
 * @param column Column index.
 * @param width Column width to set in pixels, `-1` to restore automatic
 *              column sizing.
 * @memberof uiTable
 */
API void uiTableColumnSetWidth (uiTable *t, int column, int width);

/**
 * Returns the table selection mode.
 *
 * @param t uiTable instance.
 * @returns The table selection mode. [Default `uiTableSelectionModeZeroOrOne`]
 *
 * @memberof uiTable
 */
API uiTableSelectionMode uiTableGetSelectionMode (uiTable *t);

/**
 * Sets the table selection mode.
 *
 * @param t uiTable instance.
 * @param mode Table selection mode to set.
 *
 * @warning All rows will be deselected if the existing selection is illegal
 *          in the new selection mode.
 * @memberof uiTable
 */
API void uiTableSetSelectionMode (uiTable *t, uiTableSelectionMode mode);

/**
 * Registers a callback for when the table selection changed.
 *
 * @param t uiTable instance.
 * @param f Callback function.\n
 *          @p sender Back reference to the instance that triggered the callback.\n
 *          @p senderData User data registered with the sender instance.
 * @param data User data to be passed to the callback.
 *
 * @note The callback is not triggered when calling uiTableSetSelection() or
 *       when needing to clear the selection on uiTableSetSelectionMode().
 * @note Only one callback can be registered at a time.
 * @memberof uiTable
 */
API void uiTableOnSelectionChanged (uiTable *t, void (*f) (uiTable *table, void *data), void *data);

/**
 * Returns the current table selection.
 *
 * @param t uiTable instance.
 * @returns The number of selected rows and corresponding row indices.\n
 *          Caller is responsible for freeing the data with `uiFreeTableSelection()`.
 *
 * @note For empty selections the `Rows` pointer will be NULL.
 * @memberof uiTable
 */
API uiTableSelection *uiTableGetSelection (uiTable *t);

/**
 * Sets the current table selection clearing any previous selection.
 *
 * @param t uiTable instance.
 * @param sel Table selection.\n
 *            Data is copied internally. Ownership is not transferred.
 *
 * @note Selecting more rows than the selection mode allows for results in nothing happening.
 * @note For empty selections the Rows pointer is never accessed.
 * @memberof uiTable
 */
API void uiTableSetSelection (uiTable *t, uiTableSelection *sel);

/**
 * Frees the given uiTableSelection and all it's resources.
 *
 * @param s uiTableSelection instance.
 * @memberof uiTableSelection
 */
API void uiFreeTableSelection (` Selection *s);
