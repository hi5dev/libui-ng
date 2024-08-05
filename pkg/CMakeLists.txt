cmake_minimum_required (VERSION 3.25)

project (
  ui_pkg

  DESCRIPTION
  "LibUI's redistribution packages"

  VERSION
  "${CMAKE_PROJECT_VERSION}"

  LANGUAGES
  C
)

set (PROJECT_VERSION_TWEAK "${CMAKE_PROJECT_VERSION_TWEAK}")

if (APPLE)
  add_subdirectory (osx)

elseif (LINUX)
  add_subdirectory (linux)

elseif (WIN32)
  add_subdirectory (win32)

endif ()
