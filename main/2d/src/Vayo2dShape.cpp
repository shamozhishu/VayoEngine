#include "Vayo2dShape.h"
#include "Vayo2dPivot.h"
#include "Vayo2dRenderer.h"
#include "Vayo2dLayerManager.h"
#include "Vayo2dGeometry.h"

NS_VAYO2D_BEGIN

Reflex<Shape, const wstring&, LayerManager*> Shape::_dynReflex;
Shape::Shape(const wstring& name, LayerManager* oriLayerMgr)
	: Body(name, oriLayerMgr)
	, _kind(EK_ELLIPSE)
	, _changed(false)
	, _geom(nullptr)
	, _opPathGeom(nullptr)
{
	_form.ellipse.center = Vector2df::Origin;
	_form.ellipse.radius = Vector2df(1.0f, 1.0f);
}

Shape::~Shape()
{
	resetGeometry();
}

void Shape::update(float dt)
{
	Pivot::getSingleton().getCurLayerMgr()->registerForRendering(this, getQueueID());
}

void Shape::render()
{
	Renderer* pRenderer = Pivot::getSingleton().getActiveRenderer();
	FeaturePtr feature = getFeature();
	if (_kind == EK_POINT || _kind == EK_LINE)
		feature->_fill = false;
	pRenderer->setFeature(*feature);
	if (_geom)
		pRenderer->drawGeometry(_geom);
	else
	{
		switch (_kind)
		{
		case EK_POINT:
			pRenderer->drawPoint(_form.pt); break;
		case EK_LINE:
			pRenderer->drawLine(_form.line._start, _form.line._end); break;
		case EK_RECT:
			pRenderer->drawRect(_form.rect); break;
		case EK_ELLIPSE:
			pRenderer->drawEllipse(_form.ellipse.center, _form.ellipse.radius); break;
		case EK_ROUNDED_RECT:
			pRenderer->drawRoundedRect(_form.roundedRect.rect, _form.roundedRect.radius); break;
		}
	}
}

void Shape::setPoint(const Vector2df& point)
{
	_changed = true;
	_kind = EK_POINT;
	_form.pt = point;
}

void Shape::setLine(const Line2df& line)
{
	_changed = true;
	_kind = EK_LINE;
	_form.line = line;
}

void Shape::setRect(const Rectf& rect)
{
	_changed = true;
	_kind = EK_RECT;
	_form.rect = rect;
}

void Shape::setRound(const Vector2df& center, float radius)
{
	_changed = true;
	_kind = EK_ELLIPSE;
	_form.ellipse.center = center;
	_form.ellipse.radius = Vector2df(radius, radius);
}

void Shape::setEllipse(const Vector2df& center, const Vector2df& radius)
{
	_changed = true;
	_kind = EK_ELLIPSE;
	_form.ellipse.center = center;
	_form.ellipse.radius = radius;
}

void Shape::setRoundedRect(const Rectf& rect, const Vector2df& radius)
{
	_changed = true;
	_kind = EK_ROUNDED_RECT;
	_form.roundedRect.rect = rect;
	_form.roundedRect.radius = radius;
}

bool Shape::beginFigure()
{
	_changed = true;
	_kind = EK_PATH;
	_opPathGeom = getGeometry()->geom_cast<PathGeometry>();
	return _opPathGeom && _opPathGeom->beginFigure();
}

void Shape::endFigure()
{
	if (_kind == EK_PATH && _opPathGeom)
		_opPathGeom->endFigure();
	_opPathGeom = nullptr;
}

void Shape::addLine(const Vector2df& pt)
{
	if (_kind == EK_PATH && _opPathGeom)
		_opPathGeom->addLine(pt);
}

void Shape::addLines(const Vector2df* pt, unsigned int size)
{
	if (_kind == EK_PATH && _opPathGeom)
		_opPathGeom->addLines(pt, size);
}

Geometry* Shape::getGeometry()
{
	if (!_changed && _geom)
		return _geom;

	RectGeometry* pRectGeom = nullptr;
	EllipseGeometry* pEllipseGeom = nullptr;
	RoundedRectGeometry* pRoundedRectGeom = nullptr;
	PathGeometry* pPathGeom = nullptr;
	switch (_kind)
	{
	case EK_POINT:
		pRectGeom = Pivot::getSingleton().getActiveRenderer()->createGeometry(EGT_RECT, getName())->geom_cast<RectGeometry>();
		if (pRectGeom->buildRect(Rectf(_form.pt._x, _form.pt._y, _form.pt._x, _form.pt._y)))
		{
			_geom = pRectGeom;
			_changed = false;
		}
		break;
	case EK_LINE:
		pPathGeom = Pivot::getSingleton().getActiveRenderer()->createGeometry(EGT_PATH, getName())->geom_cast<PathGeometry>();
		if (pPathGeom->beginFigure())
		{
			pPathGeom->addLine(_form.line._start);
			pPathGeom->addLine(_form.line._end);
			pPathGeom->endFigure();
			_geom = pPathGeom;
			_changed = false;
		}
		break;
	case EK_RECT:
		pRectGeom = Pivot::getSingleton().getActiveRenderer()->createGeometry(EGT_RECT, getName())->geom_cast<RectGeometry>();
		if (pRectGeom->buildRect(Rectf(_form.rect._upperLeftCorner._x, _form.rect._upperLeftCorner._y, _form.rect._lowerRightCorner._x, _form.rect._lowerRightCorner._y)))
		{
			_geom = pRectGeom;
			_changed = false;
		}
		break;
	case EK_ELLIPSE:
		pEllipseGeom = Pivot::getSingleton().getActiveRenderer()->createGeometry(EGT_ELLIPSE, getName())->geom_cast<EllipseGeometry>();
		if (pEllipseGeom->buildEllipse(_form.ellipse.center, _form.ellipse.radius))
		{
			_geom = pEllipseGeom;
			_changed = false;
		}
		break;
	case EK_ROUNDED_RECT:
		pRoundedRectGeom = Pivot::getSingleton().getActiveRenderer()->createGeometry(EGT_ROUNDED_RECT, getName())->geom_cast<RoundedRectGeometry>();
		if (pRoundedRectGeom->buildRoundedRect(_form.roundedRect.rect, _form.roundedRect.radius))
		{
			_geom = pRoundedRectGeom;
			_changed = false;
		}
		break;
	case EK_PATH:
		_geom = pPathGeom = Pivot::getSingleton().getActiveRenderer()->createGeometry(EGT_PATH, getName())->geom_cast<PathGeometry>();
		_changed = false;
		break;
	}

	return _geom;
}

void Shape::resetGeometry()
{
	if (_geom)
	{
		Pivot::getSingleton().getActiveRenderer()->destroyGeometry(_geom->getName());
		_geom = nullptr;
	}
}

NS_VAYO2D_END
