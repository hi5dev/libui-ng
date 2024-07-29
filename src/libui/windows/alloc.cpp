#include "alloc.h"

#include <ui/userbugs.h>

#include <map>
#include <sstream>

#define rawBytes(pa) (&((*pa)[0]))

static std::map<uint8_t *, byteArray *>    heap;
static std::map<byteArray *, const char *> types;

void
initAlloc ()
{
  // no-op
}

void
uninitAlloc ()
{
  std::ostringstream oss;
  std::string        ossstr;

  if (heap.empty ())
    return;

  for (const auto &alloc : heap)
    oss << static_cast<void *> (alloc.first) << " " << types[alloc.second] << "\n";

  ossstr = oss.str ();

  uiprivUserBug (
      "Some data was leaked; either you left a uiControl lying around or there's a bug in libui itself. "
      "Leaked data:\n%s",
      ossstr.c_str ());
}

void *
uiprivAlloc (const size_t size, const char *type)
{
  auto *const out    = new byteArray (size, 0);
  heap[out->data ()] = out;
  types[out]         = type;

  // ReSharper disable once CppDFALocalValueEscapesFunction
  return out->data ();
}

void *
uiprivRealloc (void *_p, const size_t size, const char *type)
{
  auto *const p = static_cast<uint8_t *> (_p);

  if (p == nullptr)
    // ReSharper disable once CppDFALocalValueEscapesFunction
    return uiprivAlloc (size, type);

  byteArray *arr = heap[p];

  arr->resize (size, 0);

  heap.erase (p);

  heap[arr->data ()] = arr;

  // ReSharper disable once CppDFALocalValueEscapesFunction
  return arr->data ();
}

void
uiprivFree (void *_p)
{
  auto *const p = static_cast<uint8_t *> (_p);

  if (p == nullptr)
    uiprivImplBug ("attempt to uiprivFree(NULL)");

  types.erase (heap[p]);

  delete heap[p];

  heap.erase (p);
}
