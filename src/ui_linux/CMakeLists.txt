cmake_minimum_required (VERSION 3.25)

add_library (ui_linux STATIC)

add_library (ui::ui ALIAS ui_linux)

target_link_libraries (
  ui_linux

  PRIVATE
  ui::linux::interface
  ui::test
)

target_sources (
  ui_linux

  PRIVATE
  ui_linux.c
  ui_linux_main.c
)

if (UI_CTEST_TARGETS)
  add_test (NAME "ui_linux_test" COMMAND ui::test::runner -n ui_linux_test)
  add_test (NAME "ui_linux_main_test" COMMAND ui::test::runner -n ui_linux_main_test)
endif ()

if (UI_INSTALL)
  install (TARGETS ui_linux)
endif ()
