#pragma once

#include "ui_event.h"

//! @brief Use this macro to guard against inserting unsupported types.
//! @param list to insert the item into.
//! @param item being inserted.
#define ui_event_list_insert(list, item)                                                                              \
  _Generic ((item),                                                                                                   \
      struct ui_event_consumer_t *: ui_event_list_insert_after,                                                       \
      struct ui_event_event_t *: ui_event_list_insert_after,                                                          \
      struct ui_event_producer_t *: ui_event_list_insert_after) (list, item)

//! @see @file ui_event.h
struct ui_event_list_t
{
  union
  {
    struct ui_event_producer_t *producer; //!< Producer data.
    struct ui_event_event_t    *event;    //!< Event data.
    struct ui_event_consumer_t *consumer; //!< Consumer data.
    void                       *data;     //!< Raw data.
  };                                      //!< Item in the list.
  struct ui_event_list_t *previous;       //!< Previous item in the list.
  struct ui_event_list_t *next;           //!< Next item in the list.
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
    void                   *data                     //!< Item being added to the list.
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
int ui_event_list_size (         //!< @params
    struct ui_event_list_t *list //!< The list.
);
