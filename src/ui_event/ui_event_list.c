#include <ui_event_list.h>

#include "ui_event_error.h"

#include <stdlib.h>

void
ui_event_list_clear (struct ui_event_list_t *list)
{
  if (list == NULL)
    return;

  for (const struct ui_event_list_t *i = ui_event_list_last (list); i != NULL; i = i->previous)
    free (i->next);

  free (list);
}

struct ui_event_list_t *
ui_event_list_first (struct ui_event_list_t *list)
{
  if (list == NULL)
    return NULL;

  struct ui_event_list_t *i = list;

  while (i->previous != NULL)
    i = i->previous;

  return i;
}

struct ui_event_list_t *
ui_event_list_insert_after (struct ui_event_list_t *list, const ui_event_data_t item)
{
  struct ui_event_list_t *i = calloc (1, sizeof (*list));
  if (i == NULL)
    {
      ui_event_perrno ();
      return NULL;
    }

  i->item = item;

  if (list != NULL)
    {
      i->previous = list;
      i->next     = list->next;
      list->next  = i;
    }

  return i;
}

struct ui_event_list_t *
ui_event_list_last (struct ui_event_list_t *list)
{
  if (list == NULL)
    return NULL;

  struct ui_event_list_t *i = list;

  while (i->next != NULL)
    i = i->next;

  return i;
}

struct ui_event_list_t *
ui_event_list_remove (struct ui_event_list_t *list)
{
  if (list == NULL)
    return NULL;

  if (list->next != NULL)
    list->next->previous = list->previous;

  if (list->previous != NULL)
    list->previous->next = list->next;

  struct ui_event_list_t *i = list->previous != NULL ? list->previous : list->next;

  free (list);

  return i;
}

int
ui_event_list_size (const struct ui_event_list_t *list)
{
  if (list == NULL)
    return 0;

  int count = 1;

  for (const struct ui_event_list_t *i = list; i != NULL; i = i->next)
    count++;

  for (const struct ui_event_list_t *i = list; i != NULL; i = i->previous)
    count++;

  return count;
}
