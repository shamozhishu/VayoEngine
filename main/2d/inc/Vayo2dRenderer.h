/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* äÖÈ¾Æ÷
\*************************************************************************/
#ifndef __VAYO2D_RENDERER_H__
#define __VAYO2D_RENDERER_H__

#include "Vayo2dSupport.h"
#include "VayoDevice.h"
#include "VayoSharedPtr.h"
#include "Vayo2dPaintbrush.h"
#include "math/VayoRectangle.h"
#include "math/VayoMatrix3x3.h"
NS_VAYO2D_BEGIN

enum ETransformKind
{
	ETK_VIEW = 0,
	ETK_WORLD,
	ETK_COUNT
};

class _Vayo2dExport Renderer
{
public:
	Renderer(const wstring& name);
	virtual ~Renderer();

	virtual SurfacePtr createSurface(const wstring& name) = 0;
	virtual PaintbrushPtr createPaintbrush(ERenderTarget rt, unsigned int devid) = 0;

	virtual Geometry* createGeometry(const wstring& name = L"") = 0;
	virtual Geometry* findGeometry(const wstring& name);
	virtual void      destroyGeometry(const wstring& name);
	virtual void      destroyAllGeometries();

	virtual bool init() = 0;
	virtual bool beginDraw(Device* drawWnd) = 0;
	virtual bool endDraw() = 0;
	virtual void drawPoint(const Vector2df& pt) = 0;
	virtual void drawLine(const Vector2df& startPt, const Vector2df& endPt) = 0;
	virtual void drawRect(const Rectf& rc) = 0;
	virtual void drawRect(float x, float y, float w, float h) = 0;
	virtual void drawEllipse(const Vector2df& center, const Vector2df& radius) = 0;
	virtual void drawGeometry(Geometry* geometry) = 0;
	virtual bool setRenderTarget(ERenderTarget rt);
	virtual const Matrix3x3& getTransform(ETransformKind kind) const = 0;
	virtual void setTransform(ETransformKind kind, const Matrix3x3& mat) = 0;
	virtual void setFeature(const Feature& feature) = 0;
	virtual Paintbrush* getPaintbrush(unsigned int devid);

protected:
	ERenderTarget _renderTarget;
	map<wstring, Geometry*> _geometries;
	PaintbrushPtr _hwndPaintbrushs[EDID_DEVICE_COUNT];
	PaintbrushPtr _bitmapPaintbrushs[EDID_DEVICE_COUNT];

protected:
	PROPERTY_R_REF(wstring, _name, Name)
};

NS_VAYO2D_END

#endif // __VAYO2D_RENDERER_H__
