cmake_minimum_required (VERSION 3.25)

project (
  ui_headless

  DESCRIPTION
  "A platform-independent headless implementation of LibUI for testing purposes."

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_library (ui_headless STATIC)

add_library (ui::headless ALIAS ui_headless)

target_link_libraries (
  ui_headless

  PRIVATE
  ui::test

  PUBLIC
  ui::headless::interface
  ui::ui
)

target_sources (
  ui_headless

  PRIVATE
  ui_headless.c
  ui_headless_main.c
)

if (TARGET ui::test::runner)
  ui_test_runner (LINK_LIBRARY ui::headless)

  add_test (NAME "ui_headless/test" COMMAND ui::test::runner -n ui_headless_test)
  add_test (NAME "ui_headless/test/main" COMMAND ui::test::runner -n ui_headless_main_test)
endif ()
