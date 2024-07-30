#include "grid.h"

#include "winpublic.h"

#include <algorithm>
#include <controlsigs.h>
#include <ui/userbugs.h>
#include <uipriv.h>

static bool
gridRecomputeMinMax (uiGrid *g)
{
  bool first = true;

  for (const gridChild *gc : *g->children)
    {
      // this is important; we want g->xmin/g->ymin to satisfy gridLayoutData::visibleRow()/visibleColumn()
      if (!uiControlVisible (gc->c))
        continue;

      if (first)
        {
          g->xmin = gc->left;
          g->ymin = gc->top;
          g->xmax = gc->left + gc->xspan;
          g->ymax = gc->top + gc->yspan;

          first = false;

          continue;
        }

      g->xmin = std::min (g->xmin, gc->left);
      g->ymin = std::min (g->ymin, gc->top);
      g->xmax = std::max (g->xmax, gc->left + gc->xspan);
      g->ymax = std::max (g->ymax, gc->top + gc->yspan);
    }

  return first;
}

class gridLayoutData
{
  int ycount;

public:
  int **gg; // topological map gg[y][x] = control index
  int  *colwidths;
  int  *rowheights;
  bool *hexpand;
  bool *vexpand;
  int   nVisibleRows;
  int   nVisibleColumns;

  bool noVisible;

  explicit
  gridLayoutData (uiGrid *g)
  {
    int x;
    int y;

    this->noVisible = gridRecomputeMinMax (g);

    this->gg = new int *[ycount (g)];
    for (y = 0; y < ycount (g); y++)
      {
        this->gg[y] = new int[xcount (g)];
        for (x = 0; x < xcount (g); x++)
          this->gg[y][x] = -1;
      }

    for (size_t i = 0; i < g->children->size (); i++)
      {
        const auto *gc = (*g->children)[i];

        if (!uiControlVisible (gc->c))
          continue;

        for (y = gc->top; y < gc->top + gc->yspan; y++)
          for (x = gc->left; x < gc->left + gc->xspan; x++)
            this->gg[toyindex (g, y)][toxindex (g, x)] = i; // NOLINT(*-narrowing-conversions)
      }

    this->colwidths = new int[xcount (g)];
    ZeroMemory (this->colwidths, xcount (g) * sizeof (int));

    this->rowheights = new int[ycount (g)];
    ZeroMemory (this->rowheights, ycount (g) * sizeof (int));

    this->hexpand = new bool[xcount (g)];
    ZeroMemory (this->hexpand, xcount (g) * sizeof (bool));

    this->vexpand = new bool[ycount (g)];
    ZeroMemory (this->vexpand, ycount (g) * sizeof (bool));

    this->ycount = ycount (g);

    // if a row or column only contains emptys and spanning cells of a opposite-direction spannings, it is invisible
    // and should not be considered for padding amount calculations note that the first row and column will always be
    // visible because gridRecomputeMinMax() computed a smallest fitting rectangle
    if (this->noVisible)
      return;

    this->nVisibleRows = 0;
    for (y = 0; y < this->ycount; y++)
      if (this->visibleRow (g, y))
        this->nVisibleRows++;

    this->nVisibleColumns = 0;
    for (x = 0; x < xcount (g); x++)
      if (this->visibleColumn (g, x))
        this->nVisibleColumns++;
  }

  ~
  gridLayoutData ()
  {
    delete[] this->hexpand;
    delete[] this->vexpand;
    delete[] this->colwidths;
    delete[] this->rowheights;

    for (int y = 0; y < this->ycount; y++)
      delete[] this->gg[y];

    delete[] this->gg;
  }

  bool
  visibleRow (const uiGrid *g, const int y) const
  {

    for (int x = 0; x < xcount (g); x++)
      if (this->gg[y][x] != -1)
        {
          const auto *gc = (*g->children)[this->gg[y][x]];

          if (gc->yspan == 1 || gc->top - g->ymin == y)
            return true;
        }

    return false;
  }

  bool
  visibleColumn (const uiGrid *g, const int x) const
  {

    for (int y = 0; y < this->ycount; y++)
      if (this->gg[y][x] != -1)
        {
          const auto *gc = (*g->children)[this->gg[y][x]];
          if (gc->xspan == 1 || gc->left - g->xmin == x)
            return true;
        }

    return false;
  }
};

static void
gridPadding (const uiGrid *g, int *xpadding, int *ypadding)
{
  uiWindowsSizing sizing;

  *xpadding = 0;
  *ypadding = 0;
  if (g->padded != 0)
    {
      uiWindowsGetSizing (g->hwnd, &sizing);
      uiWindowsSizingStandardPadding (&sizing, xpadding, ypadding);
    }
}

static void
gridRelayout (uiGrid *g)
{
  if (g->children->empty ())
    return;

  RECT r;
  uiWindowsEnsureGetClientRect (g->hwnd, &r);
  int width  = r.right - r.left;
  int height = r.bottom - r.top;

  int xpadding;
  int ypadding;
  gridPadding (g, &xpadding, &ypadding);

  const auto *ld = new gridLayoutData (g);
  if (ld->noVisible)
    {
      delete ld;
      return;
    }

  // 0) discount padding from width/height
  width -= (ld->nVisibleColumns - 1) * xpadding;
  height -= (ld->nVisibleRows - 1) * ypadding;

  // 1) compute colwidths and rowheights before handling expansion
  // we only count non-spanning controls to avoid weirdness
  for (int iy = 0; iy < ycount (g); iy++)
    for (int ix = 0; ix < xcount (g); ix++)
      {
        const int i = ld->gg[iy][ix];

        if (i == -1)
          continue;

        auto *gc = (*g->children)[i];

        int iwidth;
        int iheight;
        uiWindowsControlMinimumSize (uiWindowsControl (gc->c), &iwidth, &iheight);

        if (gc->xspan == 1)
          ld->colwidths[ix] = std::max (ld->colwidths[ix], iwidth);

        if (gc->yspan == 1)
          ld->rowheights[iy] = std::max (ld->rowheights[iy], iheight);

        gc->minwidth  = iwidth;
        gc->minheight = iheight;
      }

  // 2) figure out which rows/columns expand but not span
  // we need to know which expanding rows/columns don't span before we can handle the ones that do
  for (int i = 0; i < g->children->size (); i++)
    {
      const auto *gc = (*g->children)[i];

      if (!uiControlVisible (gc->c))
        continue;

      if (gc->hexpand != 0 && gc->xspan == 1)
        ld->hexpand[toxindex (g, gc->left)] = true;

      if (gc->vexpand != 0 && gc->yspan == 1)
        ld->vexpand[toyindex (g, gc->top)] = true;
    }

  // 3) figure out which rows/columns expand that do span
  // the way we handle this is simple: if none of the spanned rows/columns expand, make all rows/columns expand
  for (int i = 0; i < g->children->size (); i++)
    {
      const auto *gc = (*g->children)[i];
      if (!uiControlVisible (gc->c))
        continue;

      if (gc->hexpand != 0 && gc->xspan != 1)
        {
          bool doit = true;

          for (int ix = gc->left; ix < gc->left + gc->xspan; ix++)
            if (ld->hexpand[toxindex (g, ix)])
              {
                doit = false;
                break;
              }

          if (doit)
            for (int ix = gc->left; ix < gc->left + gc->xspan; ix++)
              ld->hexpand[toxindex (g, ix)] = true;
        }
      if (gc->vexpand != 0 && gc->yspan != 1)
        {
          bool doit = true;

          for (int iy = gc->top; iy < gc->top + gc->yspan; iy++)
            if (ld->vexpand[toyindex (g, iy)])
              {
                doit = false;
                break;
              }

          if (doit)
            for (int iy = gc->top; iy < gc->top + gc->yspan; iy++)
              ld->vexpand[toyindex (g, iy)] = true;
        }
    }

  // 4) compute and assign expanded widths/heights
  int nhexpand = 0;
  int nvexpand = 0;
  for (int i = 0; i < xcount (g); i++)
    if (ld->hexpand[i])
      nhexpand++;
    else
      width -= ld->colwidths[i];

  for (int i = 0; i < ycount (g); i++)
    if (ld->vexpand[i])
      nvexpand++;
    else
      height -= ld->rowheights[i];

  for (int i = 0; i < xcount (g); i++)
    if (ld->hexpand[i])
      ld->colwidths[i] = width / nhexpand;

  for (int i = 0; i < ycount (g); i++)
    if (ld->vexpand[i])
      ld->rowheights[i] = height / nvexpand;

  // 5) reset the final coordinates for the next step
  for (int i = 0; i < g->children->size (); i++)
    {
      auto *gc = (*g->children)[i];
      if (!uiControlVisible (gc->c))
        continue;

      gc->finalx      = 0;
      gc->finaly      = 0;
      gc->finalwidth  = 0;
      gc->finalheight = 0;
    }

  // 6) compute cell positions and sizes
  for (int iy = 0; iy < ycount (g); iy++)
    {
      int curx = 0;
      int prev = -1;
      for (int ix = 0; ix < xcount (g); ix++)
        {
          if (!ld->visibleColumn (g, ix))
            continue;

          const int i = ld->gg[iy][ix];
          if (i != -1)
            {
              auto *gc = (*g->children)[i];
              if (iy == toyindex (g, gc->top))
                {
                  // don't repeat this step if the control spans vertically
                  if (i != prev)
                    gc->finalx = curx;

                  else
                    gc->finalwidth += xpadding;

                  gc->finalwidth += ld->colwidths[ix];
                }
            }

          curx += ld->colwidths[ix] + xpadding;
          prev = i;
        }
    }

  for (int ix = 0; ix < xcount (g); ix++)
    {

      int cury = 0;
      int prev = -1;
      for (int iy = 0; iy < ycount (g); iy++)
        {
          if (!ld->visibleRow (g, iy))
            continue;
          const int i = ld->gg[iy][ix];
          if (i != -1)
            {
              auto *gc = (*g->children)[i];
              if (ix == toxindex (g, gc->left))
                {
                  // don't repeat this step if the control spans horizontally
                  if (i != prev)
                    gc->finaly = cury;

                  else
                    gc->finalheight += ypadding;

                  gc->finalheight += ld->rowheights[iy];
                }
            }
          cury += ld->rowheights[iy] + ypadding;
          prev = i;
        }
    }

  // 7) everything as it stands now is set for xalign == Fill yalign == Fill; set the correct alignments
  // this is why we saved minwidth/minheight above
  for (int i = 0; i < g->children->size (); i++)
    {
      auto *gc = (*g->children)[i];
      if (!uiControlVisible (gc->c))
        continue;
      if (gc->halign != uiAlignFill)
        {
          switch (gc->halign)
            {
            case uiAlignEnd:
              {
                gc->finalx += gc->finalwidth - gc->minwidth;
                break;
              }

            case uiAlignCenter:
              {
                gc->finalx += (gc->finalwidth - gc->minwidth) / 2;
                break;
              }

            default:
              break;
            }

          gc->finalwidth = gc->minwidth;
        }

      if (gc->valign != uiAlignFill)
        {
          switch (gc->valign)
            {
            case uiAlignEnd:
              {
                gc->finaly += gc->finalheight - gc->minheight;
                break;
              }

            case uiAlignCenter:
              {
                gc->finaly += (gc->finalheight - gc->minheight) / 2;
                break;
              }

            default:;
            }

          gc->finalheight = gc->minheight;
        }
    }

  // 8) and FINALLY we resize
  for (int iy = 0; iy < ycount (g); iy++)
    for (int ix = 0; ix < xcount (g); ix++)
      {
        const int i = ld->gg[iy][ix];
        if (i != -1)
          {
            // treat empty cells like spaces
            const auto *gc = (*g->children)[i];
            uiWindowsEnsureMoveWindowDuringResize (reinterpret_cast<HWND> (uiControlHandle (gc->c)), gc->finalx,
                                                   gc->finaly, gc->finalwidth, gc->finalheight);
          }
      }

  delete ld;
}

static void
uiGridDestroy (uiControl *c)
{
  auto *g = reinterpret_cast<uiGrid *> (c);

  for (gridChild *gc : *g->children)
    {
      uiControlSetParent (gc->c, nullptr);
      uiControlDestroy (gc->c);
      uiprivFree (gc);
    }

  delete g->indexof;
  delete g->children;

  uiWindowsEnsureDestroyWindow (g->hwnd);
  uiFreeControl (uiControl (g));
}

static uintptr_t
uiGridHandle (uiControl *c)
{
  return reinterpret_cast<uintptr_t> (reinterpret_cast<uiGrid *> (c)->hwnd);
}

static uiControl *
uiGridParent (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->parent;
}

static void
uiGridSetParent (uiControl *c, uiControl *parent)
{
  uiControlVerifySetParent (c, parent);
  reinterpret_cast<uiWindowsControl *> (c)->parent = parent;
}

static int
uiGridToplevel (uiControl *c)
{
  return 0;
}

static int
uiGridVisible (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->visible;
}

static void
uiGridShow (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 1;
  ShowWindow (reinterpret_cast<uiGrid *> (c)->hwnd, SW_SHOW);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static void
uiGridHide (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->visible = 0;
  ShowWindow (reinterpret_cast<uiGrid *> (c)->hwnd, SW_HIDE);
  uiWindowsControlNotifyVisibilityChanged (reinterpret_cast<uiWindowsControl *> (c));
}

static int
uiGridEnabled (uiControl *c)
{
  return reinterpret_cast<uiWindowsControl *> (c)->enabled;
}

static void
uiGridEnable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 1;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}
static void
uiGridDisable (uiControl *c)
{
  reinterpret_cast<uiWindowsControl *> (c)->enabled = 0;
  uiWindowsControlSyncEnableState (reinterpret_cast<uiWindowsControl *> (c), uiControlEnabledToUser (c));
}

static void
uiGridSyncEnableState (uiWindowsControl *c, const int enabled)
{
  auto *g = reinterpret_cast<uiGrid *> (c);

  if (uiWindowsShouldStopSyncEnableState (uiWindowsControl (g), enabled) != 0)
    return;

  for (const gridChild *gc : *g->children)
    uiWindowsControlSyncEnableState (uiWindowsControl (gc->c), enabled);
}

static void
uiGridSetParentHWND (uiWindowsControl *c, const HWND parent)
{
  uiWindowsEnsureSetParentHWND (reinterpret_cast<uiGrid *> (c)->hwnd, parent);
}

static void
uiGridMinimumSize (uiWindowsControl *c, int *width, int *height)
{
  auto *g = uiGrid (c);
  int   xpadding;
  int   ypadding;
  int   x;
  int   y;
  int   minwid;
  int   minht;

  *width  = 0;
  *height = 0;

  if (g->children->empty ())
    return;

  gridPadding (g, &xpadding, &ypadding);
  const auto *ld = new gridLayoutData (g);
  if (ld->noVisible)
    {
      // nothing to do; return 0x0
      delete ld;
      return;
    }

  // 1) compute colwidths and rowheights before handling expansion
  for (y = 0; y < ycount (g); y++)
    for (x = 0; x < xcount (g); x++)
      {
        const int i = ld->gg[y][x];
        if (i == -1)
          continue;

        gridChild *gc = (*g->children)[i];

        uiWindowsControlMinimumSize (reinterpret_cast<uiWindowsControl *> (gc->c), &minwid, &minht);

        // allot equal space in the presence of spanning to keep things sane
        ld->colwidths[x]  = std::max (ld->colwidths[x], minwid / gc->xspan);
        ld->rowheights[y] = std::max (ld->rowheights[y], minht / gc->yspan);

        // save these for step 6
        gc->minwidth  = minwid;
        gc->minheight = minht;
      }

  // 2) compute total column width/row height
  int colwidth = 0;
  for (x = 0; x < xcount (g); x++)
    colwidth += ld->colwidths[x];

  int rowheight = 0;
  for (y = 0; y < ycount (g); y++)
    rowheight += ld->rowheights[y];

  // and that's it; just account for padding
  *width  = colwidth + (ld->nVisibleColumns - 1) * xpadding;
  *height = rowheight + (ld->nVisibleRows - 1) * ypadding;
}

static void
uiGridMinimumSizeChanged (uiWindowsControl *c)
{
  auto *g = reinterpret_cast<uiGrid *> (c);

  if (uiWindowsControlTooSmall (reinterpret_cast<uiWindowsControl *> (g)) != 0)
    {
      uiWindowsControlContinueMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (g));
      return;
    }
  gridRelayout (g);
}

static void
uiGridLayoutRect (uiWindowsControl *c, RECT *r)
{
  uiWindowsEnsureGetWindowRect (reinterpret_cast<uiGrid *> (c)->hwnd, r);
}

static void
uiGridAssignControlIDZOrder (uiWindowsControl *c, LONG_PTR *controlID, HWND *insertAfter)
{
  uiWindowsEnsureAssignControlIDZOrder (reinterpret_cast<uiGrid *> (c)->hwnd, controlID, insertAfter);
}

static void
uiGridChildVisibilityChanged (uiWindowsControl *c)
{
  uiWindowsControlMinimumSizeChanged (c);
}

static void
gridArrangeChildren (uiGrid *g)
{
  if (g->children->empty ())
    return;

  const auto *ld = new gridLayoutData (g);

  auto *visited = new bool[g->children->size ()];
  ZeroMemory (visited, g->children->size () * sizeof (bool));

  for (int y = 0; y < ycount (g); y++)
    for (int x = 0; x < xcount (g); x++)
      {
        const int i = ld->gg[y][x];
        if (i == -1)
          continue;

        if (visited[i])
          continue;

        visited[i]     = true;
        const auto *gc = (*g->children)[i];

        LONG_PTR controlID   = 100;
        HWND     insertAfter = nullptr;
        uiWindowsControlAssignControlIDZOrder (uiWindowsControl (gc->c), &controlID, &insertAfter);
      }

  delete[] visited;
  delete ld;
}

static gridChild *
toChild (uiControl *c, const int xspan, const int yspan, const int hexpand, const uiAlign halign, const int vexpand,
         const uiAlign valign)
{
  if (xspan < 0)
    uiprivUserBug ("You cannot have a negative xspan in a uiGrid cell.");

  if (yspan < 0)
    uiprivUserBug ("You cannot have a negative yspan in a uiGrid cell.");

  auto *gc = uiprivNew (struct gridChild);

  gc->c       = c;
  gc->xspan   = xspan;
  gc->yspan   = yspan;
  gc->hexpand = hexpand;
  gc->halign  = halign;
  gc->vexpand = vexpand;
  gc->valign  = valign;

  return gc;
}

static void
add (uiGrid *g, gridChild *gc)
{
  uiControlSetParent (gc->c, reinterpret_cast<uiControl *> (g));

  uiWindowsControlSetParentHWND (reinterpret_cast<uiWindowsControl *> (gc->c), g->hwnd);

  g->children->push_back (gc);
  (*g->indexof)[gc->c] = g->children->size () - 1;

  gridRecomputeMinMax (g);

  gridArrangeChildren (g);

  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (g));
}

void
uiGridAppend (uiGrid *g, uiControl *c, const int left, const int top, const int xspan, const int yspan,
              const int hexpand, const uiAlign halign, const int vexpand, const uiAlign valign)
{
  gridChild *gc = toChild (c, xspan, yspan, hexpand, halign, vexpand, valign);

  gc->left = left;
  gc->top  = top;

  add (g, gc);
}

void
uiGridInsertAt (uiGrid *g, uiControl *c, uiControl *existing, const uiAt at, const int xspan, const int yspan,
                const int hexpand, const uiAlign halign, const int vexpand, const uiAlign valign)
{

  gridChild *gc = toChild (c, xspan, yspan, hexpand, halign, vexpand, valign);

  const gridChild *other = (*g->children)[(*g->indexof)[existing]];

  switch (at)
    {
    case uiAtLeading:
      {
        gc->left = other->left - gc->xspan;
        gc->top  = other->top;
        break;
      }

    case uiAtTop:
      {
        gc->left = other->left;
        gc->top  = other->top - gc->yspan;
        break;
      }

    case uiAtTrailing:
      {
        gc->left = other->left + other->xspan;
        gc->top  = other->top;
        break;
      }

    case uiAtBottom:
      {
        gc->left = other->left;
        gc->top  = other->top + other->yspan;
        break;
      }
    }
  add (g, gc);
}

int
uiGridPadded (const uiGrid *g)
{
  return g->padded;
}

void
uiGridSetPadded (uiGrid *g, const int padded)
{
  g->padded = padded;
  uiWindowsControlMinimumSizeChanged (reinterpret_cast<uiWindowsControl *> (g));
}

static void
onResize (uiWindowsControl *c)
{
  gridRelayout (reinterpret_cast<uiGrid *> (c));
}

uiGrid *
uiNewGrid ()
{
  auto *g = reinterpret_cast<uiGrid *> (uiWindowsAllocControl (sizeof (uiGrid), uiGridSignature, "uiGrid"));

  auto *control      = reinterpret_cast<uiControl *> (g);
  control->Destroy   = uiGridDestroy;
  control->Disable   = uiGridDisable;
  control->Enable    = uiGridEnable;
  control->Enabled   = uiGridEnabled;
  control->Handle    = uiGridHandle;
  control->Hide      = uiGridHide;
  control->Parent    = uiGridParent;
  control->SetParent = uiGridSetParent;
  control->Show      = uiGridShow;
  control->Toplevel  = uiGridToplevel;
  control->Visible   = uiGridVisible;

  auto *windows_control                   = reinterpret_cast<uiWindowsControl *> (g);
  windows_control->AssignControlIDZOrder  = uiGridAssignControlIDZOrder;
  windows_control->ChildVisibilityChanged = uiGridChildVisibilityChanged;
  windows_control->LayoutRect             = uiGridLayoutRect;
  windows_control->MinimumSize            = uiGridMinimumSize;
  windows_control->MinimumSizeChanged     = uiGridMinimumSizeChanged;
  windows_control->SetParentHWND          = uiGridSetParentHWND;
  windows_control->SyncEnableState        = uiGridSyncEnableState;
  windows_control->enabled                = 1;
  windows_control->visible                = 1;

  g->hwnd = uiWindowsMakeContainer (reinterpret_cast<uiWindowsControl *> (g), onResize);

  g->children = new std::vector<gridChild *>;
  g->indexof  = new std::map<uiControl *, size_t>;

  return g;
}
