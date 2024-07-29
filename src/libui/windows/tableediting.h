#pragma once

#include <windef.h>

#include <commctrl.h>

#include <ui/table.h>

extern HRESULT uiprivTableResizeWhileEditing (uiTable *t);

extern HRESULT uiprivTableHandleNM_CLICK (uiTable *t, const NMITEMACTIVATE *nm, LRESULT *lResult);

extern HRESULT uiprivTableFinishEditingText (uiTable *t);

extern HRESULT uiprivTableAbortEditingText (uiTable *t);
