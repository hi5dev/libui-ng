#include <ui_test_expect.h>
#include <ui_window.h>

#include <stdlib.h>

static ui_test_case
ui_window_cdtor_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_window_cdtor_test);

  struct ui_window_t *window = ui_window_create (__FUNCTION__, 0, 0, 0);
  ui_expect_not_null (window);
  ui_window_destroy (window);
}

static ui_test_case
ui_window_borderless_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_window_borderless_test);

  struct ui_window_t *window             = ui_window_create (__FUNCTION__, 0, 0, 0);
  const int           default_borderless = ui_window_get_borderless (window);

  ui_window_set_borderless (window, 1);
  const int set_borderless_attempt1 = ui_window_get_borderless (window);

  ui_window_set_borderless (window, 0);
  const int set_borderless_attempt2 = ui_window_get_borderless (window);

  ui_window_destroy (window);
  ui_expect_cmp (int, default_borderless, is, 0);
  ui_expect_cmp (int, set_borderless_attempt1, is, 1);
  ui_expect_cmp (int, set_borderless_attempt2, is, 0);
}

static ui_test_case
ui_window_client_size_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_window_client_size_test);

  struct ui_window_t *window = ui_window_create (__FUNCTION__, 320, 200, 0);

  ui_window_set_client_size (window, 123, 45);

  int width  = 0;
  int height = 0;
  ui_window_get_client_size (window, &width, &height);

  ui_window_destroy (window);
  ui_expect_cmp (int, width, is, 123);
  ui_expect_cmp (int, height, is, 45);
}

static ui_test_case
ui_window_focus_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_window_focus_test);

  struct ui_window_t *window = ui_window_create (__FUNCTION__, 0, 0, 0);

  const int focused = ui_window_get_focused (window);

  ui_window_destroy (window);
  ui_expect_cmp (int, focused, is, 0);
}

static ui_test_case
ui_window_fullscreen_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_window_fullscreen_test);

  struct ui_window_t *window = ui_window_create (__FUNCTION__, 0, 0, 0);

  const int initial_fullscreen = ui_window_get_fullscreen (window);

  ui_window_set_fullscreen (window, 1);
  const int set_fullscreen = ui_window_get_fullscreen (window);

  ui_window_set_fullscreen (window, 0);
  const int unset_fullscreen = ui_window_get_fullscreen (window);

  ui_window_destroy (window);
  ui_expect_cmp (int, initial_fullscreen, is, 0);
  ui_expect_cmp (int, set_fullscreen, is, 1);
  ui_expect_cmp (int, unset_fullscreen, is, 0);
}

static ui_test_case
ui_window_position_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_window_position_test);

  struct ui_window_t *window = ui_window_create (__FUNCTION__, 0, 0, 0);

  int initial_width;
  int initial_height;
  ui_window_get_client_size (window, &initial_width, &initial_height);

  int initial_x;
  int initial_y;
  ui_window_get_position (window, &initial_x, &initial_y);

  int updated_x;
  int updated_y;
  ui_window_set_position (window, 12, 34);
  ui_window_get_position (window, &updated_x, &updated_y);

  int updated_width;
  int updated_height;
  ui_window_get_client_size (window, &updated_width, &updated_height);

  ui_window_destroy (window);
  ui_expect_cmp (int, updated_x, is, 12);
  ui_expect_cmp (int, updated_y, is, 34);
  ui_expect_cmp (int, updated_width, is, initial_width);
  ui_expect_cmp (int, updated_height, is, initial_height);
}

static ui_test_case
ui_window_resizable_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_window_resizable_test);

  struct ui_window_t *window = ui_window_create (__FUNCTION__, 0, 0, 0);

  const int initial_resizable = ui_window_get_resizable (window);

  ui_window_set_resizable (window, 0);
  const int unset_resizable = ui_window_get_resizable (window);

  ui_window_set_resizable (window, 1);
  const int reset_resizable = ui_window_get_resizable (window);

  ui_window_destroy (window);
  ui_expect_cmp (int, initial_resizable, is, 1);
  ui_expect_cmp (int, unset_resizable, is, 0);
  ui_expect_cmp (int, reset_resizable, is, 1);
}

static ui_test_case
ui_window_title_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_window_title_test);

  struct ui_window_t *window        = ui_window_create (__FUNCTION__, 0, 0, 0);
  char               *initial_title = ui_window_get_title (window);

  ui_window_set_title (window, "updated title");
  char *updated_title = ui_window_get_title (window);

  ui_window_destroy (window);
  ui_expect_cmp (str, initial_title, is, __FUNCTION__);
  free (initial_title);

  ui_expect_cmp (str, updated_title, is, "updated title");
  free (updated_title);
}

static ui_test_case
ui_window_visibility_test (void)
{
  static struct ui_test_t test = ui_test (test, ui_window_visibility_test);

  struct ui_window_t *window             = ui_window_create (__FUNCTION__, 0, 0, 0);
  const int           initial_visibility = ui_window_is_visible (window);

  // move window offscreen to prevent strobing windows while testing
  ui_window_set_position (window, 0, -300);

  ui_window_show (window);
  const int visible = ui_window_is_visible (window);

  ui_window_hide (window);
  const int hidden = ui_window_is_visible (window);

  ui_window_destroy (window);
  ui_expect_cmp (int, initial_visibility, is, 0);
  ui_expect_cmp (int, visible, is, 1);
  ui_expect_cmp (int, hidden, is, 0);
}
