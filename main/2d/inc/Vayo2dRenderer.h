/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* ��Ⱦ��
\*************************************************************************/
#ifndef __VAYO2D_RENDERER_H__
#define __VAYO2D_RENDERER_H__

#include "Vayo2dSupport.h"
#include "VayoSharedPtr.h"
#include "Vayo2dPaintbrush.h"
#include "VayoRectangle.h"
#include "VayoMatrix3x3.h"
#include "Vayo2dGeometry.h"
NS_VAYO2D_BEGIN

enum ETransformKind
{
	ETK_VIEW = 0,
	ETK_WORLD,
	ETK_PROJECTION,
	ETK_COUNT
};

class _Vayo2dExport Renderer
{
public:
	Renderer(const wstring& name);
	virtual ~Renderer();

	virtual PicProcessor* getPicProcessor() = 0;
	virtual PicturePtr    createPicture(const wstring& filename) = 0;
	virtual PaintbrushPtr createPaintbrush(ERenderTarget rt, EDeviceID devid) = 0;

	virtual Geometry* createGeometry(EGeometryType type, const wstring& name = L"") = 0;
	virtual Geometry* findGeometry(const wstring& name);
	virtual void      destroyGeometry(const wstring& name);
	virtual void      destroyAllGeometries();

	virtual bool init() = 0;
	virtual bool beginDraw(Device* drawWnd) = 0;
	virtual bool endDraw() = 0;
	virtual void resize(unsigned int width, unsigned int height) = 0;
	virtual void drawPoint(const Vector2df& pt) = 0;
	virtual void drawLine(const Vector2df& startPt, const Vector2df& endPt) = 0;
	virtual void drawRect(const Rectf& rc) = 0;
	virtual void drawRect(float x, float y, float w, float h) = 0;
	virtual void drawEllipse(const Vector2df& center, const Vector2df& radius) = 0;
	virtual void drawRoundedRect(const Rectf& rect, const Vector2df& radius) = 0;
	virtual void drawGeometry(Geometry* geometry) = 0;
	virtual void drawPicture(PicturePtr pic, const Position2df& pos) = 0;
	virtual void drawPicture(PicturePtr pic, const Rectf& dstRect, const Rectf& srcRect) = 0;
	virtual bool setRenderTarget(ERenderTarget rt, const Dimension2di* pSize = nullptr) = 0;
	virtual const Matrix3x3& getTransform(ETransformKind kind) const = 0;
	virtual void setTransform(ETransformKind kind, const Matrix3x3& mat) = 0;
	virtual void setFeature(const Feature& feature) = 0;
	virtual bool isDeviceLost(EDeviceID devid = EDID_CURRENT_DEV) const;
	virtual Paintbrush* getPaintbrush(EDeviceID devid = EDID_CURRENT_DEV);

protected:
	map<wstring, Geometry*> _geometries;
	bool          _deviceLost[EDID_DEVICE_COUNT];
	PaintbrushPtr _hwndPaintbrushs[EDID_DEVICE_COUNT];
	PaintbrushPtr _bitmapPaintbrushs[EDID_DEVICE_COUNT];

protected:
	PROPERTY_R_REF(wstring, _name, Name)
	PROPERTY_R(ERenderTarget, _renderTarget, RenderTarget)
};

NS_VAYO2D_END

#endif // __VAYO2D_RENDERER_H__
