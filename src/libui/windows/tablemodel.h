#pragma once

#include <ui/table_model.h>

#include <vector>

struct uiTableModel
{
  uiTableModelHandler *mh;

  std::vector<uiTable *> *tables;
};
