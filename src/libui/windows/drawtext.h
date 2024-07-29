#pragma once

#include <vector>

#define pointSizeToDWriteSize(size) (size * (96.0 / 72.0))

struct uiDrawTextLayout
{
  IDWriteTextFormat *format;

  IDWriteTextLayout *layout;

  std::vector<drawTextBackgroundParams *> *backgroundParams;

  size_t *u8tou16;

  size_t nUTF8;

  size_t *u16tou8;

  size_t nUTF16;
};
