#pragma once

struct drawTextBackgroundParams;

// drawtext.cpp
class drawingEffectsAttr : public IUnknown {
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
	drawingEffectsAttr(void);

	// IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	void setColor(double r, double g, double b, double a);
	void setUnderline(uiUnderline u);
	void setUnderlineColor(double r, double g, double b, double a);
	HRESULT mkColorBrush(ID2D1RenderTarget *rt, ID2D1SolidColorBrush **b);
	HRESULT underline(uiUnderline *u);
	HRESULT mkUnderlineBrush(ID2D1RenderTarget *rt, ID2D1SolidColorBrush **b);
};
// TODO figure out where this type should *really* go in all the headers...
struct drawTextBackgroundParams {
	size_t start;
	size_t end;
	double r;
	double g;
	double b;
	double a;
};


extern void uiprivAttributedStringApplyAttributesToDWriteTextLayout(uiDrawTextLayoutParams *p, IDWriteTextLayout *layout, std::vector<struct drawTextBackgroundParams *> **backgroundFuncs);
