#pragma once

#include "ui_event.h"

//! @see @file ui_event.h
struct ui_event_list_t
{
  ui_event_data_t         item;     //!< Item in the list.
  struct ui_event_list_t *previous; //!< Previous item in the list.
  struct ui_event_list_t *next;     //!< Next item in the list.
};

//! @brief Clears all items from a list.
//! @remark This frees the list, but not the items stored within.
void ui_event_list_clear (       //!< @params
    struct ui_event_list_t *list //!< List to clear.
);

//! @brief Gets the first item of the given list.
//! @return The first item or @p NULL if the list is empty.
struct ui_event_list_t *ui_event_list_first ( //! @params
    struct ui_event_list_t *list              //!< A list item.
);

//! @brief Adds an item to the list.
//! @returns List data created for the given item.
struct ui_event_list_t *ui_event_list_insert_after ( //!< @params
    struct ui_event_list_t *list,                    //!< List to append the item.
    ui_event_data_t         item                     //!< Item being added to the list.
);

//! @brief Gets the last item of the given list.
//! @return The last item or @p NULL if the list is empty.
struct ui_event_list_t *ui_event_list_last ( //! @params
    struct ui_event_list_t *list             //!< A list item.
);

//! @brief Removes an item from a list.
//! @returns Either the previous item, next item, or @p NULL when the list is empty.
//! @remark This frees the given list, but not the item it references.
struct ui_event_list_t *ui_event_list_remove ( //!< @params
    struct ui_event_list_t *list               //!< List with the item being removed.
);

//! @returns Total number of items in the given list.
int ui_event_list_size (               //!< @params
    struct ui_event_list_t *list //!< The list.
);
