#include <chrono>
#include <condition_variable>
#include <mutex>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <time.h>
#include <ui/box.h>
#include <ui/button.h>
#include <ui/init.h>
#include <ui/main.h>

#include <ui/multiline_entry.h>
#include <ui/window.h>

using namespace std;

uiMultilineEntry  *e;
condition_variable cv;
mutex              m;
unique_lock<mutex> ourlock (m); // NOLINT(*-err58-cpp)
thread            *timeThread;

void
sayTime (void *data)
{
  const auto *s = static_cast<char *> (data);

  uiMultilineEntryAppend (e, s);
  delete s;
}

void
threadproc ()
{
  ourlock.lock ();
  while (cv.wait_for (ourlock, chrono::seconds (1)) == cv_status::timeout)
    {
      time_t t = time (NULL);

      const char *base = ctime (&t); // NOLINT(*-mt-unsafe)

      auto *s = new char[strlen (base) + 1];

      strcpy (s, base);
      uiQueueMain (sayTime, s);
    }
}

int
onClosing (uiWindow *w, void *data)
{
  cv.notify_all ();
  // C++ throws a hissy fit if you don't do this
  // we might as well, to ensure no uiQueueMain() gets in after uiQuit()
  timeThread->join ();
  uiQuit ();
  return 1;
}

void
saySomething (uiButton *, void *)
{
  uiMultilineEntryAppend (e, "Saying something\n");
}

int
main ()
{
  uiInitOptions o = {};

  if (uiInit (&o) != NULL)
    abort ();

  uiWindow *w = uiNewWindow ("Hello", 320, 240, 0);
  uiWindowSetMargined (w, 1);

  uiBox *b = uiNewVerticalBox ();
  uiBoxSetPadded (b, 1);
  uiWindowSetChild (w, uiControl (b));

  e = uiNewMultilineEntry ();
  uiMultilineEntrySetReadOnly (e, 1);

  uiButton *btn = uiNewButton ("Say Something");
  uiButtonOnClicked (btn, saySomething, NULL);
  uiBoxAppend (b, uiControl (btn), 0);

  uiBoxAppend (b, uiControl (e), 1);

  // timeThread needs to lock ourlock itself - see http://stackoverflow.com/a/34121629/3408572
  ourlock.unlock ();
  timeThread = new thread (threadproc);

  uiWindowOnClosing (w, onClosing, NULL);
  uiControlShow (uiControl (w));
  uiMain ();
  return 0;
}
