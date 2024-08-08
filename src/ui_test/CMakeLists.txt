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
  ui_test_assert.c
  ui_test_expect.c
  ui_test_main.c
  ui_test_report_stdout.c
  ui_test_ulp.c
)

function (ui_test UI_TARGET)
  if (NOT TARGET ui::test::runner)
    return ()
  endif ()

  if (NOT TARGET "${UI_TARGET}")
    message (FATAL_ERROR "Error in ui_test() arguments: ${UI_TARGET} is not a valid target")
  endif ()

  cmake_parse_arguments (UI "" "" "TEST_CASES" ${ARGN})

  get_target_property (UI_TEST_TARGETS ui_test_runner UI_TEST_TARGETS)
  if ("UI_TEST_TARGETS-NOTFOUND" STREQUAL "${UI_TEST_TARGETS}")
    set (UI_TEST_TARGETS "")
  endif ()

  list (FIND UI_TEST_TARGETS "${UI_TARGET}" LINKED)
  if (LINKED EQUAL -1)
    ui_test_runner (LINK_LIBRARY "${UI_TARGET}")
    list (APPEND UI_TEST_TARGETS "${UI_TARGET}")
    set_target_properties (ui_test_runner PROPERTIES UI_TEST_TARGETS "${UI_TEST_TARGETS}")
  endif ()

  list (APPEND CMAKE_MESSAGE_INDENT "  ")

  foreach (FUNCTION ${UI_TEST_CASES})
    string (REPLACE "${UI_TARGET}_" "" UI_TEST_NAME "${FUNCTION}")
    set (UI_TEST_NAME "${UI_TARGET}/${UI_TEST_NAME}")
    add_test (NAME "${UI_TEST_NAME}" COMMAND ui::test::runner -n "${FUNCTION}")
    set_tests_properties ("${UI_TEST_NAME}" PROPERTIES SKIP_REGULAR_EXPRESSION "\\[SKIP\\]")
  endforeach ()
endfunction ()

ui_test (
  ui_test

  TEST_CASES
  ui_test_assert_cmp_bool_test
  ui_test_assert_cmp_double_test
  ui_test_assert_cmp_float_test
  ui_test_assert_cmp_int_test
  ui_test_assert_cmp_string_test

  ui_test_assert_eq_bool_test

  ui_test_cmp_double_test
  ui_test_cmp_float_test
  ui_test_cmp_int_test
  ui_test_cmp_str_test

  ui_test_expect_bool_test
  ui_test_expect_null_test

  ui_test_set_status_test

  ui_test_ulp_compare_double_test
  ui_test_ulp_compare_float_test
)
