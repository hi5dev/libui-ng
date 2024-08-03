cmake_minimum_required (VERSION 3.25)

add_library (ui_osx STATIC)

add_library (ui::ui ALIAS ui_osx)

target_link_libraries (
  ui_osx

  PRIVATE
  ui::osx::interface
  ui::test
)

target_sources (
  ui_osx

  PRIVATE
  ui_osx.c
  ui_osx_main.c
)

if (UI_CTEST_TARGETS)
  add_test (NAME "ui_osx_main_test" COMMAND ui::test::runner -n ui_osx_main_test)
  add_test (NAME "ui_osx_test" COMMAND ui::test::runner -n ui_osx_test)
endif ()

if (UI_INSTALL)
  install(TARGETS ui_osx)
endif ()
