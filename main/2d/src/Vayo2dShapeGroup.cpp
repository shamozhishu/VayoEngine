#include "Vayo2dShapeGroup.h"
#include "Vayo2dShape.h"
#include "Vayo2dLayer.h"
#include "Vayo2dPivot.h"
#include "Vayo2dRenderer.h"
#include "Vayo2dGeometry.h"
#include "Vayo2dLayerManager.h"

NS_VAYO2D_BEGIN

Reflex<ShapeGroup, const wstring&, LayerManager*> ShapeGroup::_dynReflex;
ShapeGroup::ShapeGroup(const wstring& name, LayerManager* oriLayerMgr)
	: Body(name, oriLayerMgr)
	, _groupGeom(nullptr)
{
}

ShapeGroup::~ShapeGroup()
{
	destroy();
}

void ShapeGroup::update(float dt)
{
	Pivot::getSingleton().getCurLayerMgr()->registerForRendering(this, getQueueID());
}

void ShapeGroup::render()
{
	if (_groupGeom)
	{
		Renderer* pRenderer = Pivot::getSingleton().getActiveRenderer();
		pRenderer->setFeature(*getFeature());
		pRenderer->drawGeometry(_groupGeom);
	}
}

void ShapeGroup::addShape(Shape* shape, const Vector2df& position /*= Vector2df::Origin*/,
	float rotAngle /*= 0.0f*/, const Vector2df& scale /*= Vector2df(1.0f, 1.0f)*/)
{
	if (!_groupGeom)
		_groupGeom = Pivot::getSingleton().getActiveRenderer()->createGeometry(EGT_GROUP, getName())->geom_cast<GeometryGroup>();

	if (shape && _groupGeom)
		_groupGeom->addGeometry(shape->getGeometry());
}

void ShapeGroup::addLayer(Layer* layer)
{
	if (!_groupGeom)
		_groupGeom = Pivot::getSingleton().getActiveRenderer()->createGeometry(EGT_GROUP, getName())->geom_cast<GeometryGroup>();

	if (layer && _groupGeom)
	{
		list<Joint*> nodes = layer->getChildren();
		list<Joint*>::const_iterator cit = nodes.cbegin();
		for (; cit != nodes.cend(); ++cit)
		{
			Layer* pChild = dynamic_cast<Layer*>(*cit);
			if (pChild)
				addLayer(pChild);
		}

		map<wstring, Body*> bodies = layer->getBodies();
		map<wstring, Body*>::const_iterator itmap = bodies.cbegin();
		for (; itmap != bodies.cend(); ++itmap)
		{
			Shape* pShape = dynamic_cast<Shape*>(itmap->second);
			if (pShape)
				addShape(pShape);
		}
	}
}

bool ShapeGroup::build()
{
	if (!_groupGeom)
		return false;
	return _groupGeom->buildGroup();
}

void ShapeGroup::destroy()
{
	if (_groupGeom)
	{
		Pivot::getSingleton().getActiveRenderer()->destroyGeometry(_groupGeom->getName());
		_groupGeom = nullptr;
	}
}

NS_VAYO2D_END
