#pragma once

/**
 * @brief Test statuses.
 */
enum ui_test_status_t
{
  UI_TEST_STATUS_REGISTER = 0, //<! The test is being registered.
  UI_TEST_STATUS_PENDING  = 1, //<! Still waiting to run.
  UI_TEST_STATUS_RUNNING  = 2, //<! Currently in progress.
  UI_TEST_STATUS_PASSED   = 3, //<! Completed successfully.
  UI_TEST_STATUS_FAILED   = 4, //<! Was interrupted due to a failed expectation.
  UI_TEST_STATUS_SKIPPED  = 5, //<! The test was interrupted without errors using @p ui_test_skip.
};
