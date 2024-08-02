cmake_minimum_required (VERSION 3.25)

add_subdirectory (ui_test)

add_subdirectory (ui)

add_subdirectory (ui_headless)

add_subdirectory (ui_test_runner)

if (APPLE)
  add_subdirectory (ui_osx)

elseif (LINUX)
  add_subdirectory (ui_linux)

elseif (WIN32)
  add_subdirectory (ui_win32)

else ()
  message (WARNING "No implementation for ${CMAKE_SYSTEM_NAME} systems")

endif ()
