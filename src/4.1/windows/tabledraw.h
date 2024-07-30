#pragma once

#include <windef.h>

#include <ui/table.h>

extern HRESULT uiprivTableHandleNM_CUSTOMDRAW (uiTable *t, const NMLVCUSTOMDRAW *nm, LRESULT *lResult);

extern HRESULT uiprivUpdateImageListSize (uiTable *t);
