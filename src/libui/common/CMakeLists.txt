cmake_minimum_required (VERSION 3.25)

add_library (libui_common STATIC EXCLUDE_FROM_ALL)

add_library (libui::common ALIAS libui_common)

target_link_libraries (libui_common PUBLIC libui::public)

target_sources (
  libui_common

  INTERFACE FILE_SET HEADERS

  FILES
  attrstr.h
  controlsigs.h
  table.h
  uipriv.h
  utf.h
)

target_sources (
  libui_common

  PUBLIC
  areaevents.c
  attribute.c
  attrlist.c
  attrstr.c
  control.c
  debug.c
  matrix.c
  opentype.c
  shouldquit.c
  table.c
  tablemodel.c
  tablevalue.c
  userbugs.c
  utf.c
)
