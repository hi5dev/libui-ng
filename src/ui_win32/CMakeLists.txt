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

  Kernel32
  User32
)

target_sources (
  ui_win32

  PRIVATE
  ui_win32.c
  ui_win32_error.c
  ui_win32_main.c
  ui_win32_window.c
)

ui_test (
  ui_win32

  TEST_CASES
  ui_win32_get_last_error_test
  ui_win32_log_last_error_test
  ui_win32_window_create_menu_test
  ui_win32_window_register_class_test
)

if (UI_INSTALL)
  install (TARGETS ui_win32)
endif ()
