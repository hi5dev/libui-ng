#pragma once

#include <ui/control.h>

#define QA_WINDOW_WIDTH  960
#define QA_WINDOW_HEIGHT 720

uiControl *qaMakeGuide (uiControl *c, const char *text);

uiControl *buttonOnClicked ();
uiControl *checkboxOnToggled ();
uiControl *entryOnChanged ();
uiControl *passwordEntryOnChanged ();
uiControl *searchEntryOnChanged ();
uiControl *entryLongText ();
uiControl *entryOverflowText ();
uiControl *labelMultiLine ();
uiControl *radioButtonsOnSelected ();
uiControl *separatorVertical ();
uiControl *separatorHorizontal ();
uiControl *spinboxOnChanged ();
uiControl *windowFullscreen ();
uiControl *windowBorderless ();
uiControl *windowResizeable ();
uiControl *windowFullscreenBorderless ();
uiControl *windowFullscreenResizeable ();
uiControl *windowResizeableBorderless ();

const char *buttonOnClickedGuide ();
const char *checkboxOnToggledGuide ();
const char *entryOnChangedGuide ();
const char *passwordEntryOnChangedGuide ();
const char *searchEntryOnChangedGuide ();
const char *entryLongTextGuide ();
const char *entryOverflowTextGuide ();
const char *labelMultiLineGuide ();
const char *radioButtonsOnSelectedGuide ();
const char *separatorVerticalGuide ();
const char *separatorHorizontalGuide ();
const char *spinboxOnChangedGuide ();
const char *windowFullscreenGuide ();
const char *windowBorderlessGuide ();
const char *windowResizeableGuide ();
const char *windowFullscreenBorderlessGuide ();
const char *windowFullscreenResizeableGuide ();
const char *windowResizeableBorderlessGuide ();
