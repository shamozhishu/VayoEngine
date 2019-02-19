#include "Vayo2dShape.h"
#include "Vayo2dPivot.h"
#include "Vayo2dRenderer.h"
#include "Vayo2dLayerManager.h"
#include "Vayo2dGeometry.h"

NS_VAYO2D_BEGIN

Reflex<Shape, const wstring&, LayerManager*> Shape::_dynReflex;
Shape::Shape(const wstring& name, LayerManager* oriLayerMgr)
	: Body(name, oriLayerMgr)
	, _geometry(nullptr)
{
}

Shape::~Shape()
{
	if (_geometry)
		Pivot::getSingleton().getActiveRenderer()->destroyGeometry(_geometry->getName());
}

void Shape::update(float dt)
{
	Pivot::getSingleton().getCurLayerMgr()->registerForRendering(this, getQueueID());
}

void Shape::render()
{
	Renderer* pRenderer = Pivot::getSingleton().getActiveRenderer();
	pRenderer->setFeature(*getFeature());
	switch (_form.kind)
	{
	case Form::EK_POINT:
		pRenderer->drawPoint(_form.pt); break;
	case Form::EK_LINE:
		pRenderer->drawLine(_form.line._start, _form.line._end); break;
	case Form::EK_RECT:
		pRenderer->drawRect(_form.rect); break;
	case Form::EK_ELLIPSE:
		pRenderer->drawEllipse(_form.ellipse.center, _form.ellipse.radius); break;
	case Form::EK_GEOMETRY:
		pRenderer->drawGeometry(_geometry); break;
	}
}

void Shape::setPoint(const Vector2df& point)
{
	_form.kind = Form::EK_POINT;
	_form.pt = point;
	if (_geometry)
	{
		Pivot::getSingleton().getActiveRenderer()->destroyGeometry(_geometry->getName());
		_geometry = nullptr;
	}
}

void Shape::setLine(const Line2df& line)
{
	_form.kind = Form::EK_LINE;
	_form.line = line;
	if (_geometry)
	{
		Pivot::getSingleton().getActiveRenderer()->destroyGeometry(_geometry->getName());
		_geometry = nullptr;
	}
}

void Shape::setRect(const Rectf& rect)
{
	_form.kind = Form::EK_RECT;
	_form.rect = rect;
	if (_geometry)
	{
		Pivot::getSingleton().getActiveRenderer()->destroyGeometry(_geometry->getName());
		_geometry = nullptr;
	}
}

void Shape::setRound(const Vector2df& center, float radius)
{
	_form.kind = Form::EK_ELLIPSE;
	_form.ellipse.center = center;
	_form.ellipse.radius = Vector2df(radius, radius);
	if (_geometry)
	{
		Pivot::getSingleton().getActiveRenderer()->destroyGeometry(_geometry->getName());
		_geometry = nullptr;
	}
}

void Shape::setEllipse(const Vector2df& center, const Vector2df& radius)
{
	_form.kind = Form::EK_ELLIPSE;
	_form.ellipse.center = center;
	_form.ellipse.radius = radius;
	if (_geometry)
	{
		Pivot::getSingleton().getActiveRenderer()->destroyGeometry(_geometry->getName());
		_geometry = nullptr;
	}
}

Geometry* Shape::getGeometry()
{
	_form.kind = Form::EK_GEOMETRY;
	if (!_geometry)
		_geometry = Pivot::getSingleton().getActiveRenderer()->createGeometry(getName());
	return _geometry;
}

Shape::Form& Shape::Form::operator=(const Form& form)
{
	if (this == &form)
		return *this;

	kind = form.kind;
	switch (kind)
	{
	case EK_POINT: pt = form.pt; break;
	case EK_LINE: line = form.line; break;
	case EK_RECT: rect = form.rect; break;
	case EK_ELLIPSE: ellipse = form.ellipse; break;
	}

	return *this;
}

NS_VAYO2D_END
