cmake_minimum_required (VERSION 3.25)

if (UI_BUILD_TESTS)
  add_subdirectory (ui_test)
  add_subdirectory (ui_headless)
endif ()

add_subdirectory (ui)

if (APPLE)
  add_subdirectory (ui_osx)

elseif (LINUX)
  add_subdirectory (ui_linux)

elseif (WIN32)
  add_subdirectory (ui_win32)

else ()
  message (WARNING "No interface for ${CMAKE_SYSTEM_NAME} systems")

endif ()
