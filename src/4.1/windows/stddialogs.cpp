#include <windows.h>

#include <shobjidl.h>

#include "debug.h"
#include "utf16.h"
#include "window.h"

#include <ui/window.h>
#include <uipriv.h>

#define windowHWND(w) (w ? (HWND)uiControlHandle (uiControl (w)) : nullptr)

static constexpr auto COMMON_OPTSADD
    = FOS_ALLNONSTORAGEITEMS | // Enables the user to choose any item in the Shell namespace.
      FOS_DEFAULTNOMINIMODE |  // Open save-as dialogs in expanded mode.
      FOS_FORCESHOWHIDDEN |    // Include hidden and system items.
      FOS_NOCHANGEDIR |        // Don't change the current working directory.
      FOS_NODEREFERENCELINKS | // Shortcuts should not be treated as their target items.
      FOS_NOTESTFILECREATE |   // Do not test for potential file creation errors.
      FOS_NOVALIDATE |         // Ignore situations that would prevent an application from opening a file.
      FOS_SHAREAWARE |         // Call the application back through OnShareViolation for sharing violations.
      0;

char *
commonItemDialog (const HWND parent, REFCLSID clsid, REFIID iid, const FILEOPENDIALOGOPTIONS optsadd)
{
  IFileDialog          *d = nullptr;
  FILEOPENDIALOGOPTIONS opts;
  IShellItem           *result = nullptr;
  WCHAR                *wname  = nullptr;
  char                 *name   = nullptr;

  HRESULT hr = CoCreateInstance (clsid, nullptr, CLSCTX_INPROC_SERVER, iid, reinterpret_cast<LPVOID *> (&d));
  if (hr != S_OK)
    {
      (void)logHRESULT (L"error creating common item dialog", hr);
      goto out;
    }

  hr = d->GetOptions (&opts);
  if (hr != S_OK)
    {
      (void)logHRESULT (L"error getting current options", hr);
      goto out;
    }

  opts |= optsadd;
  opts &= ~FOS_NOREADONLYRETURN;
  hr = d->SetOptions (opts);
  if (hr != S_OK)
    {
      (void)logHRESULT (L"error setting options", hr);
      goto out;
    }

  hr = d->Show (parent);
  if (hr == HRESULT_FROM_WIN32 (ERROR_CANCELLED))
    goto out;

  if (hr != S_OK)
    {
      (void)logHRESULT (L"error showing dialog", hr);
      goto out;
    }

  hr = d->GetResult (&result);
  if (hr != S_OK)
    {
      (void)logHRESULT (L"error getting dialog result", hr);
      goto out;
    }

  hr = result->GetDisplayName (SIGDN_FILESYSPATH, &wname);
  if (hr != S_OK)
    {
      (void)logHRESULT (L"error getting filename", hr);
      goto out;
    }

  name = toUTF8 (wname);

out:
  if (wname != nullptr)
    CoTaskMemFree (wname);

  if (result != nullptr)
    result->Release ();

  if (d != nullptr)
    d->Release ();

  return name;
}

char *
uiOpenFile (uiWindow *parent)
{
  static constexpr auto optsadd = //
      COMMON_OPTSADD |            // Default options for all dialogs.
      FOS_FILEMUSTEXIST |         // The item returned must exist.
      FOS_PATHMUSTEXIST |         // The item returned must be in an existing folder.
      0;

  disableAllWindowsExcept (parent);

  char *res = commonItemDialog (windowHWND (parent), CLSID_FileOpenDialog, IID_IFileOpenDialog, optsadd);

  enableAllWindowsExcept (parent);

  return res;
}

char *
uiOpenFolder (uiWindow *parent)
{
  static constexpr auto optsadd = //
      COMMON_OPTSADD |            // Default options for all dialogs.
      FOS_NOTESTFILECREATE |      // Do not test for potential file creation errors.
      FOS_PATHMUSTEXIST |         // The item returned must be in an existing folder.
      FOS_PICKFOLDERS |           // Present an Open dialog that offers a choice of folders rather than files.
      0;

  disableAllWindowsExcept (parent);

  char *res = commonItemDialog (windowHWND (parent), CLSID_FileOpenDialog, IID_IFileOpenDialog, optsadd);

  enableAllWindowsExcept (parent);
  return res;
}

char *
uiSaveFile (uiWindow *parent)
{
  static constexpr auto optsadd = //
      COMMON_OPTSADD |            // Default options for all dialogs.
      FOS_OVERWRITEPROMPT |       // Prompt before overwriting an existing file of the same name.
      0;

  disableAllWindowsExcept (parent);

  char *res = commonItemDialog (windowHWND (parent), CLSID_FileSaveDialog, IID_IFileSaveDialog, optsadd);

  enableAllWindowsExcept (parent);

  return res;
}

static void
msgbox (const HWND parent, const char *title, const char *description, const TASKDIALOG_COMMON_BUTTON_FLAGS buttons,
        const PCWSTR icon)
{
  WCHAR *wtitle       = toUTF16 (title);
  WCHAR *wdescription = toUTF16 (description);

  const HRESULT hr = TaskDialog (parent, nullptr, nullptr, wtitle, wdescription, buttons, icon, nullptr);
  if (hr != S_OK)
    (void)logHRESULT (L"error showing task dialog", hr);

  uiprivFree (wdescription);
  uiprivFree (wtitle);
}

void
uiMsgBox (uiWindow *parent, const char *title, const char *description)
{
  disableAllWindowsExcept (parent);

  msgbox (windowHWND (parent), title, description, TDCBF_OK_BUTTON, nullptr);

  enableAllWindowsExcept (parent);
}

void
uiMsgBoxError (uiWindow *parent, const char *title, const char *description)
{
  disableAllWindowsExcept (parent);

  msgbox (windowHWND (parent), title, description, TDCBF_OK_BUTTON, TD_ERROR_ICON);

  enableAllWindowsExcept (parent);
}
