#pragma once

/**
 * @brief Reports the progress and status of tests.
 */
struct ui_test_reporter_t
{
  /**
   * @brief Reports that a test completed successfully.
   * @param test that passed.
   */
  void (*passed) (struct ui_test_t *test);

  /**
   * @brief Reports that a test was interrupted due to an failed test.
   * @param test that failed.
   */
  void (*failed) (struct ui_test_t *test);

  /**
   * @brief Reports that a test is about to run.
   * @param test that is about to run.
   */
  void (*running) (struct ui_test_t *test);
};
