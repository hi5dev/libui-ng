cmake_minimum_required (VERSION 3.25)

project (
  ui_win32

  DESCRIPTION
  "Win32 implementation of LibUI"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

add_library (ui_win32 STATIC)

add_library (ui::ui ALIAS ui_win32)

target_link_libraries (
  ui_win32

  PRIVATE
  ui::test

  PUBLIC
  ui
  ui::win32::interface
)

target_sources (
  ui_win32

  PRIVATE
  ui_win32.c
  ui_win32_main.c
  ui_win32_window.c
)

if (TARGET ui::test::runner)
  ui_test_runner (LINK_LIBRARY ui::ui)

  add_test (NAME "ui_win32/test" COMMAND ui::test::runner -n ui_win32_test)
  add_test (NAME "ui_win32/test/main" COMMAND ui::test::runner -n ui_win32_main_test)
  add_test (NAME "ui_win32/test/get_last_error" COMMAND ui::test::runner -n ui_win32_get_last_error_test)
endif ()

if (UI_INSTALL)
  install(TARGETS ui_win32)
endif ()
