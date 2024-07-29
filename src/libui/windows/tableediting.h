#pragma once

extern HRESULT uiprivTableResizeWhileEditing (uiTable *t);
extern HRESULT uiprivTableHandleNM_CLICK (uiTable *t, NMITEMACTIVATE *nm, LRESULT *lResult);
extern HRESULT uiprivTableFinishEditingText (uiTable *t);
extern HRESULT uiprivTableAbortEditingText (uiTable *t);
