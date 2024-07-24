cmake_minimum_required (VERSION 3.25)

find_package (PkgConfig REQUIRED)

pkg_check_modules (GTK3 REQUIRED IMPORTED_TARGET gtk+-3.0)

add_library (libui_unix STATIC)

add_library (libui::libui ALIAS libui_unix)

set_target_properties (libui_unix PROPERTIES OUTPUT_NAME libui)

target_link_libraries (libui_unix PRIVATE PkgConfig::GTK3 libui::common PUBLIC libui::public)

target_sources (
  libui_unix

  PRIVATE
  alloc.c
  area.c
  attrstr.c
  box.c
  button.c
  cellrendererbutton.c
  checkbox.c
  child.c
  colorbutton.c
  combobox.c
  control.c
  datetimepicker.c
  debug.c
  draw.c
  drawmatrix.c
  drawpath.c
  drawtext.c
  editablecombo.c
  entry.c
  fontbutton.c
  fontmatch.c
  form.c
  future.c
  graphemes.c
  grid.c
  group.c
  image.c
  label.c
  main.c
  menu.c
  multilineentry.c
  opentype.c
  progressbar.c
  radiobuttons.c
  separator.c
  slider.c
  spinbox.c
  stddialogs.c
  tab.c
  table.c
  tablemodel.c
  text.c
  util.c
  window.c
)

if (INSTALL_TARGETS)
  install (TARGETS libui_unix)
endif ()
