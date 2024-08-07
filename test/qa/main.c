#include "qa.h"

#include <ui/box.h>
#include <ui/combobox.h>
#include <ui/init.h>
#include <ui/main.h>
#include <ui/multiline_entry.h>
#include <ui/separator.h>
#include <ui/window.h>

#include <stdio.h>

struct controlTestCase
{
  const char *name;
  uiControl  *(*init) (void);
  const char *(*guide) (void);
};

struct controlTestGroup
{
  const char             *name;
  struct controlTestCase *testCases;
};

#define QA_TEST(desciption, name)                                                                                     \
  {                                                                                                                   \
    desciption, name, name##Guide                                                                                     \
  }

struct controlTestCase buttonTestCases[] = {
  QA_TEST ("1. Button OnClicked Callback", buttonOnClicked), { NULL, NULL, NULL }
};

struct controlTestCase checkboxTestCases[] = {
  QA_TEST ("1. Checkbox OnToggled Callback", checkboxOnToggled), { NULL, NULL, NULL }
};

struct controlTestCase entryTestCases[] = {
  QA_TEST ("1. Entry OnChanged Callback", entryOnChanged),
  QA_TEST ("2. Password Entry OnChanged Callback", passwordEntryOnChanged),
  QA_TEST ("3. Search Entry OnChanged Callback", searchEntryOnChanged),
  QA_TEST ("4. Entry Long Text", entryLongText),
  QA_TEST ("5. Entry Overflow Text", entryOverflowText),
  { NULL, NULL, NULL }
};

struct controlTestCase labelTestCases[] = {
  QA_TEST ("1. Multi Line", labelMultiLine), { NULL, NULL, NULL }
};

struct controlTestCase radioButtonsTestCases[] = {
  QA_TEST ("1. OnSelected Callback", radioButtonsOnSelected), { NULL, NULL, NULL }
};

struct controlTestCase separatorTestCases[] = {
  QA_TEST ("1. Vertical Separator", separatorVertical),
  QA_TEST ("2. Horizontal Separator", separatorHorizontal),
  { NULL, NULL, NULL }
};

struct controlTestCase spinboxTestCases[] = {
  QA_TEST ("1. Spinbox OnChanged Callback", spinboxOnChanged), { NULL, NULL, NULL }
};

struct controlTestCase windowTestCases[] = {
  QA_TEST ("1. Fullscreen", windowFullscreen),
  QA_TEST ("2. Borderless", windowBorderless),
  QA_TEST ("3. Resizable", windowResizeable),
  QA_TEST ("4. Fullscreen + Borderless", windowFullscreenBorderless),
  QA_TEST ("5. Fullscreen + Resizeable", windowFullscreenResizeable),
  QA_TEST ("6. Resizeable + Borderless", windowResizeableBorderless),
  { NULL, NULL, NULL }
};

struct controlTestGroup controlTestGroups[] = {
  { "uiButton",       buttonTestCases       },
  { "uiCheckbox",     checkboxTestCases     },
  { "uiEntry",        entryTestCases        },
  { "uiLabel",        labelTestCases        },
  { "uiRadioButtons", radioButtonsTestCases },
  { "uiSeparator",    separatorTestCases    },
  { "uiSpinbox",      spinboxTestCases      },
  { "uiWindow",       windowTestCases       },
};

static uiControl *qaBox = NULL;

uiBox *mainBox = NULL;
uiBox *chooserBox;

uiCombobox *testCaseChooser = NULL;

int testGroupIndex = -1;

uiControl *
qaGuide ()
{
  uiBox *box = uiNewVerticalBox ();

  uiMultilineEntry *guide = uiNewMultilineEntry ();
  uiMultilineEntrySetText (
      guide,
      "Welcome to quality assurance!\n"
      "\n"
      "Presented here are various manual test to ensure uiControls behave as expected.\n"
      "\n"
      "Usage:\n"
      "1.\tSelect the desired uiControl to test from the drop down menu.\n"
      "2.\tSelect a test case from the second drop down menu.\n"
      "3.\tFollow the step by step instructions in the right hand pane.\n"
      "4.\tIf the behavior or visual output does not match the description please report this upstream.\n"
      "\n"
      "Reporting a bug:\n"
      "\tPlease check if a bug report describing the issue you found already exists.\n"
      "\tIf that is the case, please leave a comment confirming the issue.\n"
      "\tIf you can not find an existing report, please file a new one.\n"
      "\n"
      "\tPlease include:\n"
      "\t- Library version or git commit hash\n"
      "\t- Operating system(s)\n"
      "\t- uiControl\n"
      "\t- Test case\n"
      "\t- Step number that failed\n"
      "\n"
      "\te.g.:\n"
      "\tQA: 803389e3 > Windows 7 > uiLabel > 1. Multi Line > Step 6\n"
      "\n"
      "\tSubmit the report at https://github.com/libui-ng/libui-ng/issues");

  uiBoxAppend (box, uiControl (guide), 1);
  uiMultilineEntrySetReadOnly (guide, 1);

  return uiControl (box);
}

int
onClosing (uiWindow *, void *)
{
  uiQuit ();
  return 1;
}

void
clearCurrentBox ()
{
  if (qaBox != NULL)
    {
      uiBoxDelete (mainBox, 2);
      uiControlDestroy (qaBox);
      qaBox = NULL;
    }
}

void
testCaseOnSelected (uiCombobox *sender, void *)
{
  clearCurrentBox ();

  const struct controlTestCase testCase = controlTestGroups[testGroupIndex].testCases[uiComboboxSelected (sender)];

  qaBox = qaMakeGuide (testCase.init (), testCase.guide ());

  uiBoxAppend (mainBox, qaBox, 1);
}

void
testGroupOnSelected (uiCombobox *sender, void *senderData)
{
  clearCurrentBox ();

  testGroupIndex = uiComboboxSelected (sender) - 1;

  if (testGroupIndex == -1)
    {
      qaBox = qaGuide ();
      uiBoxAppend (mainBox, qaBox, 1);
      uiControlHide (uiControl (testCaseChooser));
    }

  else
    {
      int                    i = 0;
      struct controlTestCase testCase;

      uiComboboxClear (testCaseChooser);
      while (testCase = controlTestGroups[testGroupIndex].testCases[i], testCase.name != NULL)
        {
          uiComboboxAppend (testCaseChooser, testCase.name);
          i++;
        }

      uiControlShow (uiControl (testCaseChooser));

      if (uiComboboxNumItems (testCaseChooser) > 0)
        {
          uiComboboxSetSelected (testCaseChooser, 0);
          testCaseOnSelected (testCaseChooser, NULL);
        }
    }
}

int
main (void)
{
  uiInitOptions o = { 0 };

  const char *err = uiInit (&o);
  if (err != NULL)
    {
      (void)fprintf (stderr, "error initializing ui: %s\n", err);
      uiFreeInitError (err);
      return 1;
    }

  uiWindow *w = uiNewWindow ("Quality Assurance", QA_WINDOW_WIDTH, QA_WINDOW_HEIGHT, 1);
  uiWindowSetMargined (w, 1);
  uiWindowOnClosing (w, onClosing, NULL);

  mainBox = uiNewVerticalBox ();
  uiBoxSetPadded (mainBox, 1);
  uiWindowSetChild (w, uiControl (mainBox));

  chooserBox = uiNewHorizontalBox ();
  uiBoxSetPadded (chooserBox, 1);
  uiBoxAppend (mainBox, uiControl (chooserBox), 0);
  uiBoxAppend (mainBox, uiControl (uiNewHorizontalSeparator ()), 0);

  uiCombobox *testGroupChooser = uiNewCombobox ();
  uiBoxAppend (chooserBox, uiControl (testGroupChooser), 0);

  testCaseChooser = uiNewCombobox ();
  uiBoxAppend (chooserBox, uiControl (testCaseChooser), 0);
  uiComboboxOnSelected (testCaseChooser, testCaseOnSelected, NULL);

  uiComboboxAppend (testGroupChooser, "QA Guide");
  for (size_t i = 0; i < sizeof (controlTestGroups) / sizeof (*controlTestGroups); ++i)
    uiComboboxAppend (testGroupChooser, controlTestGroups[i].name);

  uiComboboxSetSelected (testGroupChooser, 0);
  uiComboboxOnSelected (testGroupChooser, testGroupOnSelected, NULL);
  testGroupOnSelected (testGroupChooser, mainBox);

  uiControlShow (uiControl (w));
  uiMain ();
  uiUninit ();

  return 0;
}
