cmake_minimum_required (VERSION 3.25)

add_library (libui_public INTERFACE EXCLUDE_FROM_ALL)

add_library (libui::public ALIAS libui_public)

target_sources (libui_public INTERFACE FILE_SET HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")

if (APPLE)
  target_sources (libui_public INTERFACE FILE_SET HEADERS FILES ui_darwin.h)

elseif (LINUX)
  target_sources (libui_public INTERFACE FILE_SET HEADERS FILES ui_unix.h)

elseif (WIN32)
  target_sources (libui_public INTERFACE FILE_SET HEADERS FILES ui_win32.h)

endif ()

add_subdirectory (ui)
