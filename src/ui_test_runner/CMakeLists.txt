cmake_minimum_required (VERSION 3.25)

project (
  ui_test_runner

  DESCRIPTION
  "LibUI test runner"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_executable (ui_test_runner)

add_executable (ui::test::runner ALIAS ui_test_runner)

target_sources (
  ui_test_runner

  PRIVATE
  ui_test_runner.c
)

function (ui_test_runner)
  cmake_parse_arguments (UI_TEST_RUNNER "NO_WHOLE_ARCHIVE;WHOLE_ARCHIVE" "LINK_LIBRARY" "" ${ARGN})

  if (UI_TEST_RUNNER_WHOLE_ARCHIVE)
    target_link_options (ui_test_runner PRIVATE -Wl,--whole-archive)
  endif ()

  if (UI_TEST_RUNNER_NO_WHOLE_ARCHIVE)
    target_link_options (ui_test_runner PRIVATE -Wl,--no-whole-archive)
  endif ()

  if (NOT "${UI_TEST_RUNNER_LINK_LIBRARY}" STREQUAL "")
    target_link_libraries (ui_test_runner PRIVATE "${UI_TEST_RUNNER_LINK_LIBRARY}")
    target_link_options (ui_test_runner PRIVATE "$<TARGET_FILE:${UI_TEST_RUNNER_LINK_LIBRARY}>" )
  endif ()
endfunction ()
