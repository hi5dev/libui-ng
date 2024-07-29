#pragma once

#include <winuser.h>

enum
{
  msgCOMMAND = WM_APP + 0x40, //<!
  msgNOTIFY,                  //<!
  msgHSCROLL,                 //<!
  msgQueued,                  //<!
  msgD2DScratchPaint,         //<!
  msgD2DScratchLButtonDown,   //<!
};
