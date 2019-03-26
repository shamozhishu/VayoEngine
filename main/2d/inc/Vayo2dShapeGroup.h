/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 形状群
\*************************************************************************/
#ifndef __VAYO2D_SHAPE_GROUP_H__
#define __VAYO2D_SHAPE_GROUP_H__

#include "Vayo2dBody.h"
#include "Vayo2dGraphics.h"
#include "VayoVector2d.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport ShapeGroup : public Body
{
	static Reflex<ShapeGroup, const wstring&, LayerManager*> _dynReflex;
public:
	ShapeGroup(const wstring& name, LayerManager* oriLayerMgr);
	virtual ~ShapeGroup();
	virtual void update(float dt);
	virtual void addShape(Shape* shape, const Vector2df& position = Vector2df::Origin,
		float rotAngle = 0.0f, const Vector2df& scale = Vector2df(1.0f, 1.0f));
	virtual void addLayer(Layer* layer);
	virtual bool build();
	virtual void destroy();

private:
	class Region : public Graphics
	{
	public:
		Region();
		~Region();
		void render();
	private:
		PROPERTY_R(GeometryGroup*, _geomGroup, GeomGroup)
	};
	vector<Region*> _regions;

private:
	Region* getRegion(Shape* pShape);
};

NS_VAYO2D_END

#endif // __VAYO2D_SHAPE_GROUP_H__
