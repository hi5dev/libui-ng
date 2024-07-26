cmake_minimum_required (VERSION 3.25)

target_sources (
  libui_public

  INTERFACE FILE_SET HEADERS

  FILES
  area.h
  areaevents.h
  attribute.h
  attributed_string.h
  box.h
  button.h
  checkbox.h
  color_button.h
  combobox.h
  control.h
  date_time_picker.h
  dialogs.h
  draw.h
  editable_combobox.h
  entry.h
  font_button.h
  font_descriptor.h
  form.h
  grid.h
  group.h
  image.h
  init.h
  label.h
  main.h
  menu.h
  menu_item.h
  multiline_entry.h
  opentype.h
  progressbar.h
  radio_buttons.h
  separator.h
  slider.h
  spinbox.h
  tab.h
  table.h
  table_model.h
  table_value.h
  unix.h
  userbugs.h
  window.h
)

if (INSTALL_TARGETS)
  install (TARGETS libui_public FILE_SET HEADERS)
endif ()
