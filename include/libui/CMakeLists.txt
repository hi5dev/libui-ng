cmake_minimum_required (VERSION 3.25)

add_library (libui_public INTERFACE EXCLUDE_FROM_ALL)

add_library (libui::public ALIAS libui_public)

if (INSTALL_TARGETS)
  install (TARGETS libui_public FILE_SET HEADERS)
endif ()

if (APPLE)
  target_sources (libui_public INTERFACE FILE_SET HEADERS FILES ui.h ui_darwin.h)

elseif (LINUX)
  target_sources (libui_public INTERFACE FILE_SET HEADERS FILES ui.h ui_unix.h)

elseif (WIN32)
  target_sources (libui_public INTERFACE FILE_SET HEADERS FILES ui.h ui_windows.h)

endif ()
