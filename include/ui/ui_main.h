#pragma once

/**
 * @brief The user-interface's main entry-point.
 * @remark This is a thread-blocking call.
 */
void ui_main (void);

/**
 * @brief Breaks the main loop.
 */
void ui_main_quit (void);

/**
 * @brief Dispatches one user-interface message.
 * @param wait non-zero to block the calling thread until at least one message is received.
 */
void ui_main_step (int wait);

/**
 * @brief The user-interface's main loop.
 * @remark This is a thread-blocking call.
 */
void ui_main_steps (void);
