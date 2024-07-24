cmake_minimum_required (VERSION 3.25)

add_library (libui_darwin STATIC)

add_library (libui::libui ALIAS libui_darwin)

set_target_properties (libui_darwin PROPERTIES OUTPUT_NAME libui)

target_link_libraries (libui_darwin PRIVATE libui::common PUBLIC libui::public)

target_sources (
  libui_darwin

  PRIVATE
  aat.m
  alloc.m
  area.m
  areaevents.m
  attrstr.m
  autolayout.m
  box.m
  button.m
  checkbox.m
  colorbutton.m
  combobox.m
  control.m
  datetimepicker.m
  debug.m
  draw.h
  draw.m
  drawtext.m
  editablecombo.m
  entry.m
  event.m
  fontbutton.m
  fontmatch.m
  fonttraits.m
  fontvariation.m
  form.m
  future.m
  graphemes.m
  grid.m
  group.m
  image.m
  label.m
  main.m
  menu.m
  multilineentry.m
  nstextfield.m
  opentype.m
  progressbar.m
  radiobuttons.m
  scrollview.m
  separator.m
  slider.m
  spinbox.m
  stddialogs.m
  tab.m
  table.m
  tablecolumn.m
  text.m
  undocumented.m
  util.m
  window.m
  winmoveresize.m
)

if (INSTALL_TARGETS)
  install (TARGETS libui_darwin)
endif ()
