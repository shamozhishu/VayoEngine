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
{
}

ShapeGroup::~ShapeGroup()
{
	destroy();
}

void ShapeGroup::update(float dt)
{
	vector<Region*>::iterator it = _regions.begin();
	for (; it != _regions.end(); ++it)
		Pivot::getSingleton().getCurLayerMgr()->registerForRendering(*it, getQueueID());
}

void ShapeGroup::addShape(Shape* shape, const Vector2df& position /*= Vector2df::Origin*/,
	float rotAngle /*= 0.0f*/, const Vector2df& scale /*= Vector2df(1.0f, 1.0f)*/)
{
	Region* pRegion = getRegion(shape);
	if (pRegion)
		pRegion->getGeomGroup()->addGeometry(shape->getGeometry());
}

void ShapeGroup::addLayer(Layer* layer)
{
	if (!layer)
		return;

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

bool ShapeGroup::build()
{
	vector<Region*>::iterator it = _regions.begin();
	for (; it != _regions.end(); ++it)
	{
		if (!(*it)->getGeomGroup()->buildGroup())
			return false;
	}
	return true;
}

void ShapeGroup::destroy()
{
	vector<Region*>::iterator it = _regions.begin();
	for (; it != _regions.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	_regions.clear();
}

ShapeGroup::Region* ShapeGroup::getRegion(Shape* pShape)
{
	if (!pShape)
		return nullptr;

	Region* pRegion;
	vector<Region*>::iterator it = _regions.begin();
	for (; it != _regions.end(); ++it)
	{
		pRegion = *it;
		if (*(pRegion->getFeature()) == *(pShape->getFeature()))
			return pRegion;
	}

	pRegion = new Region();
	*(pRegion->getFeature()) = *(pShape->getFeature());
	_regions.push_back(pRegion);
	return pRegion;
}

//////////////////////////////////////////////////////////////////////////
ShapeGroup::Region::Region()
{
	_geomGroup = Pivot::getSingleton().getActiveRenderer()->createGeometry(EGT_GROUP)->geom_cast<GeometryGroup>();
}

ShapeGroup::Region::~Region()
{
	if (_geomGroup)
		Pivot::getSingleton().getActiveRenderer()->destroyGeometry(_geomGroup->getName());
}

void ShapeGroup::Region::render()
{
	if (_geomGroup)
	{
		Renderer* pRenderer = Pivot::getSingleton().getActiveRenderer();
		pRenderer->setFeature(*getFeature());
		pRenderer->drawGeometry(_geomGroup);
	}
}

NS_VAYO2D_END
