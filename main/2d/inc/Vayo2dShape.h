/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 形状
\*************************************************************************/
#ifndef __VAYO2D_ROUND_H__
#define __VAYO2D_ROUND_H__

#include "Vayo2dBody.h"
#include "Vayo2dGraphics.h"
#include "math/VayoLine2d.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Shape : public Body, public Graphics
{
	static Reflex<Shape, const wstring&, LayerManager*> _dynReflex;
public:
	Shape(const wstring& name, LayerManager* oriLayerMgr);
	~Shape();
	virtual void update(float dt);
	virtual void render();
	virtual void setPoint(const Vector2df& point);
	virtual void setLine(const Line2df& line);
	virtual void setRect(const Rectf& rect);
	virtual void setRound(const Vector2df& center, float radius);
	virtual void setEllipse(const Vector2df& center, const Vector2df& radius);
	virtual Geometry* getGeometry();

private:
	struct Form
	{
		enum EKind
		{
			EK_POINT,
			EK_LINE,
			EK_RECT,
			EK_ELLIPSE,
			EK_GEOMETRY
		} kind;
		union
		{
			Vector2df pt;
			Line2df line;
			Rectf rect;
			struct Ellipse
			{
				Vector2df center;
				Vector2df radius;
				Ellipse() : radius(1, 1) {}
			} ellipse;
		};
		Form() : kind(EK_ELLIPSE) {}
		~Form() {}
		Form(const Form& form) { *this = form; }
		Form& operator=(const Form& form);
	} _form;
	Geometry* _geometry;
};

NS_VAYO2D_END

#endif // __VAYO2D_ROUND_H__
