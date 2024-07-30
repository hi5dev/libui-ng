#pragma once

typedef struct uiprivTimer
{
  int   (*f) (void *);
  void *data;
} uiprivTimer;

extern int registerMessageFilter ();

extern void unregisterMessageFilter ();

extern void uiprivFreeTimer (uiprivTimer *t);

extern void uiprivUninitTimers ();
