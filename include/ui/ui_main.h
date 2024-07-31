#pragma once

struct ui_main_t;

/**
 * @brief @p ui_main_t callback function type.
 * @param ui_main @p ui_main_t
 */
typedef void (ui_main_cb_t) (struct ui_main_t *ui_main);

/**
 * @brief Platform-dependent application data.
 */
struct ui_main_t
{
  /**
   * @brief Exit code to use when terminating the application.
   */
  int exit_code;

  /**
   * @brief Non-zero while the application is running.
   */
  int running;

  /**
   * @brief Platform-dependent application data.
   */
  void *data;

  /**
   * @brief Dispatches one user-interface message.
   */
  ui_main_cb_t *dispatch;

  /**
   * @brief Blocks the calling thread until a user-interface message is ready to dispatch.
   * @remark Also determines when its time to quit running.
   */
  ui_main_cb_t *update;
};

/**
 * @brief Runs the application.
 */
void ui_main_run (struct ui_main_t *ui_main);

/**
 * @brief Breaks the main application loop started by a call to @p run.
 */
void ui_main_quit (struct ui_main_t *ui_main);

/**
 * @see ui_main_t#dispatch
 */
void ui_main_dispatch (struct ui_main_t *ui_main);

/**
 * @see ui_main_t#update
 */
void ui_main_update (struct ui_main_t *ui_main);
