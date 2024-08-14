#include <ui_event_list.h>
#include <ui_test_expect.h>

#include "ui_event_error.h"

#include <stdlib.h>

void
ui_event_list_clear (struct ui_event_list_t *list)
{
  if (list == NULL)
    return;

  for (const struct ui_event_list_t *i = ui_event_list_last (list); i != NULL; i = i->previous)
    free (i->next);
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
  if (list == NULL)
    return i;

  i->previous = list;
  i->next = list->next;

  if (list->next != NULL)
    list->next->previous = i;

  list->next = i;

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

  if (list->previous != NULL)
    list->previous->next = list->next;

  if (list->next != NULL)
    list->next->previous = list->previous;

  struct ui_event_list_t *i = NULL;

  if (list->previous != NULL)
    i = list->previous;

  else
    i = list->next;

  free (list);

  return i;
}

int
ui_event_list_size (struct ui_event_list_t *list)
{
  int count = 0;

  for (const struct ui_event_list_t *i = ui_event_list_first (list); i != NULL; i = i->next)
    count++;

  return count;
}

static ui_test_case
ui_event_list_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_event_list_test);

  const char *item1 = "Item 1";
  const char *item2 = "Item 2";
  const char *item3 = "Item 3";

  struct ui_event_list_t *cur = NULL;
  ui_expect_cmp (int, ui_event_list_size (cur), is, 0);

  // list = [Item1]
  cur = ui_event_list_insert_after (cur, (ui_event_data_t)item1);
  ui_expect_not_null (cur);
  ui_expect_cmp (str, (const char *)cur->item, is, item1);
  ui_expect_cmp (int, ui_event_list_size (cur), is, 1);
  ui_expect_null (cur->previous);
  ui_expect_null (cur->next);

  // list = [Item1, Item3] // with cursor at Item2
  cur = ui_event_list_insert_after (cur, (ui_event_data_t)item3);
  ui_expect_not_null (cur);
  ui_expect_cmp (str, (const char *)cur->item, is, item3);
  ui_expect_cmp (int, ui_event_list_size (cur), is, 2);

  ui_expect_not_null (cur->previous);
  ui_expect_cmp (str, (const char *)cur->previous->item, is, item1);
  ui_expect_cmp (int, ui_event_list_size (cur->previous), is, 2);

  ui_expect_null (cur->previous->previous);

  ui_expect_not_null (cur->previous->next);
  ui_expect_cmp (str, (const char *)cur->previous->next->item, is, item3);
  ui_expect_null (cur->next);

  cur = ui_event_list_insert_after (cur->previous, (ui_event_data_t)item2);
  ui_expect_cmp (str, (const char *)cur->item, is, item2);
  ui_expect_cmp (str, (const char *)cur->previous->item, is, item1);
  ui_expect_cmp (str, (const char *)cur->next->item, is, item3);

  ui_expect_cmp (int, ui_event_list_size (cur), is, 3);
  ui_expect_cmp (int, ui_event_list_size (cur->previous), is, 3);
  ui_expect_cmp (int, ui_event_list_size (cur->next), is, 3);

  cur = ui_event_list_remove (cur->next); // Item2
  ui_expect_cmp (int, ui_event_list_size (cur), is, 2);

  ui_event_list_clear (cur);
}
