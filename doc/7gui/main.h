#pragma once

#if defined(WIN32)
#include <windows.h>
#define main(...) WINAPI wWinMain (const HINSTANCE, HINSTANCE, LPWSTR, int)
#endif

#include <ui_main.h>
