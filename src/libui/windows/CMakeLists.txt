cmake_minimum_required (VERSION 3.25)

add_subdirectory (manifest)

add_library (libui_win32 STATIC)

add_library (libui::libui ALIAS libui_win32)

set_target_properties (libui_win32 PROPERTIES OUTPUT_NAME libui)

target_link_libraries (libui_win32 PRIVATE libui::common PUBLIC libui::public)

target_compile_options (libui_win32 PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:-Wno-int-to-pointer-cast>")

target_compile_definitions (libui_win32 PUBLIC -D UNICODE -D_UNICODE)

libui_generate_manifest (
  libui_win32

  ASSEMBLY_NAME
  "github.hi5dev.${CMAKE_PROJECT_NAME}"

  ASSEMBLY_VERSION
  "${CMAKE_PROJECT_VERSION}.0"

  ASSEMBLY_DESCRIPTION
  "${CMAKE_PROJECT_DESCRIPTION}"

  MANIFEST_OUTPUT
  "${CMAKE_PROJECT_NAME}.manifest"

  RESOURCE_OUTPUT
  "${CMAKE_PROJECT_NAME}.rc"
)

target_link_libraries (
  libui_win32

  PUBLIC
  comctl32
  comdlg32
  d2d1
  dwrite
  gdi32
  kernel32
  msimg32
  ole32
  oleacc
  oleaut32
  user32
  uuid
  uxtheme
  windowscodecs
)

target_sources (
  libui_win32

  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/resources.rc"

  PRIVATE
  alloc.cpp
  area.cpp
  areadraw.cpp
  areaevents.cpp
  areascroll.cpp
  areautil.cpp
  attrstr.cpp
  box.cpp
  button.cpp
  checkbox.cpp
  colorbutton.cpp
  colordialog.cpp
  combobox.cpp
  container.cpp
  control.cpp
  d2dscratch.cpp
  datetimepicker.cpp
  debug.cpp
  draw.cpp
  drawmatrix.cpp
  drawpath.cpp
  drawtext.cpp
  dwrite.cpp
  editablecombo.cpp
  entry.cpp
  events.cpp
  fontbutton.cpp
  fontdialog.cpp
  fontmatch.cpp
  form.cpp
  graphemes.cpp
  grid.cpp
  group.cpp
  image.cpp
  init.cpp
  label.cpp
  main.cpp
  menu.cpp
  multilineentry.cpp
  opentype.cpp
  parent.cpp
  progressbar.cpp
  radiobuttons.cpp
  separator.cpp
  sizing.cpp
  slider.cpp
  spinbox.cpp
  stddialogs.cpp
  tab.cpp
  table.cpp
  tabledispinfo.cpp
  tabledraw.cpp
  tableediting.cpp
  tablemetrics.cpp
  tabpage.cpp
  text.cpp
  utf16.cpp
  utilwin.cpp
  window.cpp
  winpublic.cpp
  winutil.cpp
)

if (INSTALL_TARGETS)
  install (TARGETS libui_win32)
endif ()
