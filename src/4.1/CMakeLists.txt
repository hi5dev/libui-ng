cmake_minimum_required (VERSION 3.25)

add_subdirectory (common)

if (APPLE)
  add_subdirectory (darwin)

elseif (LINUX)
  add_subdirectory (unix)

elseif (WIN32)
  add_subdirectory (windows)

else ()
  message (FATAL_ERROR "Unsupported system: ${CMAKE_SYSTEM_NAME}")

endif ()
