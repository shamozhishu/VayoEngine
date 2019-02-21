/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* Direct2D��Ⱦ��
\*************************************************************************/
#ifndef __D2D_RENDERER_H__
#define __D2D_RENDERER_H__

#include "D2DSupport.h"
#include "Vayo2dRenderer.h"
#include "Vayo2dFeature.h"

class D2DRenderer : public Renderer
{
public:
	D2DRenderer(const wstring& name);
	~D2DRenderer();

	SurfacePtr    createSurface(const wstring& name);
	Geometry*     createGeometry(const wstring& name = L"");
	PaintbrushPtr createPaintbrush(ERenderTarget rt, unsigned int devid);

	bool init();
	bool beginDraw(Device* drawWnd);
	bool endDraw();
	void drawPoint(const Vector2df& pt);
	void drawLine(const Vector2df& startPt, const Vector2df& endPt);
	void drawRect(const Rectf& rc);
	void drawRect(float x, float y, float w, float h);
	void drawEllipse(const Vector2df& center, const Vector2df& radius);
	void drawGeometry(Geometry* geometry);
	void drawBitmap(const Position2df& pos);
	void drawBitmap(const Rectf& dstRect, const Rectf& srcRect);
	bool setRenderTarget(ERenderTarget rt);
	const Matrix3x3& getTransform(ETransformKind kind) const;
	void setTransform(ETransformKind kind, const Matrix3x3& mat);
	void setFeature(const Feature& feature);
	ID2D1HwndRenderTarget* getHwndRT(int devid) const;
	ID2D1BitmapRenderTarget* getBitmapRT(int devid) const;

private:
	void discardDeviceResources();
	void setPaintbrushRenderState(D2DPaintbrush* pBrush);

	Feature                  _curFeature;
	Feature                  _lastFeature;
	unsigned int             _activateRTID;
	Matrix3x3                _matrizes[ETK_COUNT];
	ID2D1HwndRenderTarget*   _hwndRT[EDID_DEVICE_COUNT];
	ID2D1BitmapRenderTarget* _bitmapRT[EDID_DEVICE_COUNT];
private:
	PROPERTY_R(ID2D1RenderTarget*,  _activateRT,    ActivateRT)
	PROPERTY_R(ID2D1Factory*,       _d2dFactory,    D2DFactory)
	PROPERTY_R(IWICImagingFactory*, _wicFactory,    WICFactory)
	PROPERTY_R(IDWriteFactory*,     _dwriteFactory, DWriteFactory)
};

#endif // __D2D_RENDERER_H__
