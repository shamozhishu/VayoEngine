/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* Direct2DäÖÈ¾Æ÷
\*************************************************************************/
#ifndef __D2D_RENDERER_H__
#define __D2D_RENDERER_H__

#include "D2DSupport.h"
#include "Vayo2dRenderer.h"
#include "Vayo2dFeature.h"
#include "D2DPicProcessor.h"

class D2DRenderer : public Renderer
{
public:
	D2DRenderer(const wstring& name);
	~D2DRenderer();

	PicProcessor* getPicProcessor();
	PicturePtr    createPicture(const wstring& filename);
	Geometry*     createGeometry(const wstring& name = L"");
	PaintbrushPtr createPaintbrush(ERenderTarget rt, EDeviceID devid);

	bool init();
	bool beginDraw(Device* drawWnd);
	bool endDraw();
	void resize(unsigned int width, unsigned int height);
	void drawPoint(const Vector2df& pt);
	void drawLine(const Vector2df& startPt, const Vector2df& endPt);
	void drawRect(const Rectf& rc);
	void drawRect(float x, float y, float w, float h);
	void drawEllipse(const Vector2df& center, const Vector2df& radius);
	void drawGeometry(Geometry* geometry);
	void drawPicture(PicturePtr pic, const Position2df& pos);
	void drawPicture(PicturePtr pic, const Rectf& dstRect, const Rectf& srcRect);
	bool setRenderTarget(ERenderTarget rt, const Dimension2di* pSize = nullptr);
	const Matrix3x3& getTransform(ETransformKind kind) const;
	void setTransform(ETransformKind kind, const Matrix3x3& mat);
	void setFeature(const Feature& feature);
	ComPtr<ID2D1HwndRenderTarget> getHwndRT(EDeviceID devid = EDID_CURRENT_DEV) const;
	ComPtr<ID2D1BitmapRenderTarget> getBitmapRT(EDeviceID devid = EDID_CURRENT_DEV) const;

private:
	bool rebuildDeviceResources();
	void discardDeviceResources(unsigned int rtid);
	void setPaintbrushRenderState(D2DPaintbrush* pBrush);

	Feature                         _curFeature;
	Feature                         _lastFeature;
	D2DPicProcessor                 _picProcessor;
	Matrix3x3                       _matrizes[ETK_COUNT];
	ComPtr<ID2D1HwndRenderTarget>   _hwndRT[EDID_DEVICE_COUNT];
	ComPtr<ID2D1BitmapRenderTarget> _bitmapRT[EDID_DEVICE_COUNT];
private:
	PROPERTY_R(EDeviceID,                  _activateRTID,   ActivateRTID)
	PROPERTY_R(ComPtr<ID2D1RenderTarget>,  _activateRT,       ActivateRT)
	PROPERTY_R(ComPtr<ID2D1Factory>,       _d2dFactory,       D2DFactory)
	PROPERTY_R(ComPtr<IWICImagingFactory>, _wicFactory,       WICFactory)
	PROPERTY_R(ComPtr<IDWriteFactory>,     _dwriteFactory, DWriteFactory)
};

#endif // __D2D_RENDERER_H__
