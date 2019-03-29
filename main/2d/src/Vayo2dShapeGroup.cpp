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

void ShapeGroup::addShape(Shape* shape, const Matrix3x3& mat /*= IdentityMat3*/)
{
	Region* pRegion = getRegion(shape);
	if (!pRegion)
		return;
	Geometry* pGeom = shape->getGeometry();
	GeometryGroup* pGeomGroup = pRegion->getGeomGroup();
	if (!pGeom || !pGeomGroup)
		return;
	pGeom->transformed(mat);
	pGeomGroup->addGeometry(pGeom);
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
			addShape(pShape, layer->getAbsTransform());
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

	pRegion = new Region(this);
	*(pRegion->getFeature()) = *(pShape->getFeature());
	_regions.push_back(pRegion);
	return pRegion;
}

//////////////////////////////////////////////////////////////////////////
ShapeGroup::Region::Region(ShapeGroup* shapeGroup)
	: _shapeGroup(shapeGroup)
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

void ShapeGroup::Region::getWorldTransform(Matrix3x3& mat) const
{
	Layer* layer = _shapeGroup ? _shapeGroup->getParentLayer() : nullptr;
	if (layer)
		mat = layer->getAbsTransform();
	else
		Graphics::getWorldTransform(mat);
}

NS_VAYO2D_END
