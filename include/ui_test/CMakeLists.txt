cmake_minimum_required (VERSION 3.25)

add_library (ui_test_interface INTERFACE EXCLUDE_FROM_ALL)

add_library (ui::test::interface ALIAS ui_test_interface)

set_target_properties (
  ui_test_interface

  PROPERTIES
  LINKER_LANGUAGE C
)

target_sources (
  ui_test_interface

  INTERFACE FILE_SET HEADERS

  FILES
  ui_test.h
  ui_test_backtrace.h
  ui_test_expect.h
  ui_test_main.h
  ui_test_options.h
  ui_test_report.h
  ui_test_report_event.h
  ui_test_report_message.h
  ui_test_report_stdout.h
  ui_test_status.h
)
