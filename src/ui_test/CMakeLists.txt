cmake_minimum_required (VERSION 3.25)

project (
  ui_test

  DESCRIPTION
  "LibUI test framework implementation"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_library (ui_test STATIC)

add_library (ui::test ALIAS ui_test)

target_link_libraries (
  ui_test

  PUBLIC
  ui::test::interface
)

target_sources (
  ui_test

  PRIVATE
  ui_test.c
  ui_test_expect.c
  ui_test_main.c
  ui_test_report_stdout.c
)

function (add_test)
  _add_test (${ARGN})
  cmake_parse_arguments (TEST "" "NAME" "" ${ARGN})
  set_tests_properties (${TEST_NAME} PROPERTIES SKIP_REGULAR_EXPRESSION "\\[SKIP\\]")
endfunction ()

if (TARGET ui::test::runner)
  ui_test_runner (LINK_LIBRARY ui::test)

  add_test (NAME "ui_test/set_status" COMMAND ui::test::runner -n ui_test_set_status_test)
  add_test (NAME "ui_test/cmp/double" COMMAND ui::test::runner -n ui_test_cmp_double_test)
  add_test (NAME "ui_test/cmp/float" COMMAND ui::test::runner -n ui_test_cmp_float_test)
  add_test (NAME "ui_test/cmp/int" COMMAND ui::test::runner -n ui_test_cmp_int_test)
  add_test (NAME "ui_test/cmp/str" COMMAND ui::test::runner -n ui_test_cmp_str_test)
endif ()
