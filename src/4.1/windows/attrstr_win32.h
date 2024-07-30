#pragma once

#include <windows.h>

#include <d2d1.h>

#include <ui/attribute.h>
#include <ui/draw.h>

#include <vector>

typedef struct drawTextBackgroundParams
{
  size_t start;

  size_t end;

  double r;

  double g;

  double b;

  double a;
} drawTextBackgroundParams;

class drawingEffectsAttr final : public IUnknown
{
  ULONG refcount;

  bool hasColor;

  double r;

  double g;

  double b;

  double a;

  bool hasUnderline;

  uiUnderline u;

  bool hasUnderlineColor;

  double ur;

  double ug;

  double ub;

  double ua;

public:
  virtual ~drawingEffectsAttr () = default;
           drawingEffectsAttr ();

  HRESULT STDMETHODCALLTYPE QueryInterface (REFIID riid, void **ppvObject) override;

  ULONG STDMETHODCALLTYPE AddRef () override;

  ULONG STDMETHODCALLTYPE Release () override;

  void setColor (double r, double g, double b, double a);

  void setUnderline (uiUnderline u);

  void setUnderlineColor (double r, double g, double b, double a);

  HRESULT mkColorBrush (ID2D1RenderTarget *rt, ID2D1SolidColorBrush **b) const;

  HRESULT underline (uiUnderline *u) const;

  HRESULT mkUnderlineBrush (ID2D1RenderTarget *rt, ID2D1SolidColorBrush **b) const;
};

extern void
uiprivAttributedStringApplyAttributesToDWriteTextLayout (uiDrawTextLayoutParams *p, IDWriteTextLayout *layout,
                                                         std::vector<drawTextBackgroundParams *> **backgroundFuncs);
