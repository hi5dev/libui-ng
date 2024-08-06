#pragma once

/**
 * @brief Cells application data.
 */
struct ui_cells_t;

/**
 * @brief @ref ui_cell_t Cell types.
 */
enum ui_cell_type_t
{
  UI_CELL_VALUE_EMPTY     = 0, //!< @brief The cell has no data.
  UI_CELL_VALUE_REFERENCE = 1, //!< @brief The cell references another.
  UI_CELL_VALUE_FORMULA   = 2, //!< @brief The cell contains a formula.
  UI_CELL_VALUE_STRING    = 3, //!< @brief The cell contains a manually entered string.
  UI_CELL_VALUE_NUMBER    = 4, //!< @brief The cell contains a manually entered number.
};

/**
 * @brief State of a @ref ui_cell_t cell.
 */
enum ui_cell_state_t
{
  UI_CELL_STATE_OK,       //!< @brief The cell's data is ready to present to the user.
  UI_CELL_STATE_BUSY,     //!< @brief The cell is being evaluated.
  UI_CELL_STATE_ERROR,    //!< @brief There's an error in the cell's formula, or its reference.
  UI_CELL_STATE_MODIFIED, //!< @brief The user has modified the cell, and it has not yet been updated.
};

/**
 * @brief @ref ui_cell_t Cell error types.
 */
enum ui_cell_error_t
{
  UI_CELL_ERROR_NONE      = 0, //!< @brief No errors.
  UI_CELL_ERROR_FORMULA   = 1, //!< @brief There is an error in the cell's formula.
  UI_CELL_ERROR_RECURSIVE = 2, //!< @brief The cell has an infinitely recursive reference.
};

/**
 * @brief Address of a @ref ui_cell_t cell.
 */
struct ui_cell_address_t
{
  int column; //!< @brief Column index from left-to-right, starting at zero.
  int row;    //!< @brief Row index from top-to-bottom, starting at zero.
};

/**
 * @brief Cell data.
 */
struct ui_cell_t
{
  //! @brief The cell's address, or placement, in the spreadsheet.
  struct ui_cell_address_t address;

  //! @brief The cell's current @ref ui_cell_state_t state.
  enum ui_cell_state_t state;

  //! @brief State the the cell's error.
  enum ui_cell_error_t error;

  //! @brief Type of data stored in the cell.
  enum ui_cell_type_t type;

  //! @brief Data stored in the cell.
  union
  {
    //! @brief Used by @p UI_CELL_VALUE_REFERENCE and @p UI_CELL_VALUE_EMPTY cells.
    //! @remark @p NULL when the cell is a @p UI_CELL_VALUE_EMPTY type.
    struct ui_cell_t *reference;

    //! @brief Used by @p UI_CELL_VALUE_FORMULA cells.
    const char *formula;

    //! @brief Used by @p UI_CELL_VALUE_STRING cells.
    const char *string;

    //! @brief Used by @p UI_CELL_VALUE_NUMBER cells.
    double number;
  } data;

  //!< @brief Information about an action required on the user's part for the cell - e.g. an error message.
  const char *hint;
};

/**
 * @brief Evaluates the given mathematical formula.
 * @param[in] formula mathematical formula to evaluate.
 * @param[out] error the character position of the error in the formula, or zero on success.
 * @return Result of the evaluated formula, or NaN on error.
 */
double ui_cells_evaluate (const char *formula, int *error);

/**
 * @brief Copies the given cell data into the spreadsheet.
 * @param cells application data.
 * @param cell data to add to the spreadsheet.
 * @return The @p ui_cell_t added to the spreadsheet.
 */
struct ui_cell_t *ui_cells_add (struct ui_cells_t *cells, struct ui_cell_t cell);

/**
 * @brief Gets the cell at the given address.
 * @param cells application data.
 * @param address of the cell.
 * @return @p NULL if the cell has never been assigned a value, or a @p ui_cell_t if it has.
 */
struct ui_cell_t *ui_cells_get (struct ui_cells_t *cells, struct ui_cell_address_t *address);

/**
 * @brief Gets the total number of columns available in the spreadsheet.
 * @param cells @p ui_cells_t
 * @return Maximum number of columns in the spreadsheet.
 */
int ui_cells_n_columns (struct ui_cells_t *cells);

/**
 * @brief Gets the total number of rows available in the spreadsheet.
 * @param cells @p ui_cells_t
 * @return Maximum number of rows in the spreadsheet.
 */
int ui_cells_n_rows (struct ui_cells_t *cells);

/**
 * @brief Runs the Cells application.
 * @param cells @p ui_cells_t
 * @return non-zero when exiting with an error.
 */
int ui_cells_main (struct ui_cells_t *cells);
