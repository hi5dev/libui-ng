#pragma once

#include <windows.h>

extern BOOL runWM_COMMAND (WPARAM wParam, LPARAM lParam, LRESULT *lResult);

extern BOOL runWM_NOTIFY (WPARAM wParam, LPARAM lParam, LRESULT *lResult);

extern BOOL runWM_HSCROLL (WPARAM wParam, LPARAM lParam, LRESULT *lResult);

extern void issueWM_WININICHANGE (WPARAM wParam, LPARAM lParam);
