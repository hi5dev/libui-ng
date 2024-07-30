cmake_minimum_required (VERSION 3.25)

if (APPLE)
  add_subdirectory (osx)

elseif (LINUX)
  add_subdirectory (linux)

elseif (WIN32)
  add_subdirectory (win32)

endif ()
