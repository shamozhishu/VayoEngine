/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 形状
\*************************************************************************/
#ifndef __VAYO2D_SHAPE_H__
#define __VAYO2D_SHAPE_H__

#include "Vayo2dBody.h"
#include "Vayo2dGraphics.h"
#include "VayoLine2d.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Shape : public Body, public Graphics
{
	static Reflex<Shape, const wstring&, LayerManager*> _dynReflex;
public:
	enum EKind
	{
		EK_POINT,
		EK_LINE,
		EK_RECT,
		EK_ELLIPSE,
		EK_ROUNDED_RECT,
		EK_PATH
	};

public:
	Shape(const wstring& name, LayerManager* oriLayerMgr);
	~Shape();
	virtual void update(float dt);
	virtual void render();
	virtual void getWorldTransform(Matrix3x3& mat) const;
	virtual void setPoint(const Vector2df& point);
	virtual void setLine(const Line2df& line);
	virtual void setRect(const Rectf& rect);
	virtual void setRound(const Vector2df& center, float radius);
	virtual void setEllipse(const Vector2df& center, const Vector2df& radius);
	virtual void setRoundedRect(const Rectf& rect, const Vector2df& radius);
	virtual bool beginFigure();
	virtual void endFigure();
	virtual void addLine(const Vector2df& pt);
	virtual void addLines(const Vector2df* pt, unsigned int size);
	virtual Geometry* getGeometry();
	virtual void resetGeometry();

private:
	union Form
	{
		struct
		{
			Vector2df center;
			Vector2df radius;
		} ellipse;
		struct
		{
			Rectf rect;
			Vector2df radius;
		} roundedRect;
		Vector2df  pt;
		Line2df  line;
		Rectf    rect;
		Form() {}
	} _form;

	EKind         _kind;
	Geometry*     _geom;
	bool          _changed;
	PathGeometry* _opPathGeom;
};

NS_VAYO2D_END

#endif // __VAYO2D_SHAPE_H__
