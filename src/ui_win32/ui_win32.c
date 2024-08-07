#include <windows.h>

#include <ui_win32.h>
#include <ui_win32_error.h>

#include <assert.h>

void
ui_win32_dispatch (struct ui_win32_t *ui_win32)
{
  assert (ui_win32 != NULL);

  // Translates virtual-key messages into character messages. The character messages are posted to the calling thread's
  // message queue; to be read the next time the thread calls the GetMessage or PeekMessage function. The return value
  // is non-zero when the message is translated, and can safely be ignored here.
  (void)TranslateMessage (&ui_win32->message);

  // Dispatch the message retrieved by the GetMessage function to its window procedure. The return value specifies the
  // value returned by the window procedure, and can safely be ignored here.
  (void)DispatchMessage (&ui_win32->message);
}

void
ui_win32_update (struct ui_win32_t *ui_win32)
{
  assert (ui_win32 != NULL);

  // Retrieves a message from the calling thread's message queue. The function dispatches incoming sent messages until
  // a posted message is available for retrieval.
  const BOOL result = GetMessage (&ui_win32->message, NULL, 0, 0);

  // If there is an error, GetMessage returns -1. For example, the function fails if hWnd is an invalid window handle
  // or lpMsg is an invalid pointer. This should be considered a critical error, and abort the process immediately
  // to avoid undefined behavior.
  if (result == -1)
    ui_win32_abort_on_error ("GetMessage");

  // If GetMessage retrieves a message other than WM_QUIT, the return value is non-zero.
  if (result == 0)
    ui_win32->quit = TRUE;
}
