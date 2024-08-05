cmake_minimum_required (VERSION 3.25)

project (
  ui_implementation

  DESCRIPTION
  "LibUI implementation libraries"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

if (UI_ENABLE_TESTING)
  add_subdirectory (ui_test_runner)
  ui_test_runner (WHOLE_ARCHIVE)
endif ()

add_subdirectory (ui_test)

add_subdirectory (ui)
add_subdirectory (ui_headless)

if (APPLE)
  add_subdirectory (ui_osx)

elseif (LINUX)
  add_subdirectory (ui_linux)

elseif (WIN32)
  add_subdirectory (ui_win32)

else ()
  message (WARNING "No implementation for ${CMAKE_SYSTEM_NAME} systems")

endif ()

if (TARGET ui_test_runner)
  ui_test_runner (NO_WHOLE_ARCHIVE)
endif ()
