#include "D2DGeometry.h"
#include "D2DRenderer.h"

D2DGeometry::D2DGeometry(D2DRenderer* renderer)
	: _renderer(renderer)
{
}

//////////////////////////////////////////////////////////////////////////
D2DRectGeometry::D2DRectGeometry(const wstring& name, D2DRenderer* renderer)
	: RectGeometry(name)
	, D2DGeometry(renderer)
{
}

bool D2DRectGeometry::buildRect(const Rectf& rect)
{
	return SUCCEEDED(_renderer->getD2DFactory()->CreateRectangleGeometry(D2D1::RectF(rect._upperLeftCorner._x, rect._upperLeftCorner._y,
		rect._lowerRightCorner._x, rect._lowerRightCorner._y), &_rectGeometry));
}

ComPtr<ID2D1Geometry> D2DRectGeometry::getD2DGeometry() const
{
	return _rectGeometry;
}

//////////////////////////////////////////////////////////////////////////
D2DRoundedRectGeometry::D2DRoundedRectGeometry(const wstring& name, D2DRenderer* renderer)
	: RoundedRectGeometry(name)
	, D2DGeometry(renderer)
{
}

bool D2DRoundedRectGeometry::buildRoundedRect(const Rectf& rect, const Vector2df& radius)
{
	return SUCCEEDED(_renderer->getD2DFactory()->CreateRoundedRectangleGeometry(
		D2D1::RoundedRect(D2D1::RectF(rect._upperLeftCorner._x, rect._upperLeftCorner._y,
			rect._lowerRightCorner._x, rect._lowerRightCorner._y), radius._x, radius._y), &_roundedRectGeometry));
}

ComPtr<ID2D1Geometry> D2DRoundedRectGeometry::getD2DGeometry() const
{
	return _roundedRectGeometry;
}

//////////////////////////////////////////////////////////////////////////
D2DEllipseGeometry::D2DEllipseGeometry(const wstring& name, D2DRenderer* renderer)
	: EllipseGeometry(name)
	, D2DGeometry(renderer)
{
}

bool D2DEllipseGeometry::buildRound(const Vector2df& center, float radius)
{
	return SUCCEEDED(_renderer->getD2DFactory()->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(center._x, center._y),
		radius, radius), &_ellipseGeometry));
}

bool D2DEllipseGeometry::buildEllipse(const Vector2df& center, const Vector2df& radius)
{
	return SUCCEEDED(_renderer->getD2DFactory()->CreateEllipseGeometry(D2D1::Ellipse(D2D1::Point2F(center._x, center._y),
		radius._x, radius._y), &_ellipseGeometry));
}

ComPtr<ID2D1Geometry> D2DEllipseGeometry::getD2DGeometry() const
{
	return _ellipseGeometry;
}

//////////////////////////////////////////////////////////////////////////
D2DGeometryGroup::D2DGeometryGroup(const wstring& name, D2DRenderer* renderer)
	: GeometryGroup(name)
	, D2DGeometry(renderer)
{
}

bool D2DGeometryGroup::buildGroup()
{
	unsigned int size = _geoms.size();
	vector<ID2D1Geometry*> d2dGeoms;
	for (unsigned int i = 0; i < size; ++i)
	{
		D2DGeometry* pGeom = dynamic_cast<D2DGeometry*>(_geoms[i]);
		if (pGeom)
		{
			ID2D1Geometry* pD2DGeom = pGeom->getD2DGeometry().Get();
			if (pD2DGeom)
				d2dGeoms.push_back(pD2DGeom);
		}	
	}

	if (d2dGeoms.size() == 0)
		return false;
	return SUCCEEDED(_renderer->getD2DFactory()->CreateGeometryGroup(D2D1_FILL_MODE_ALTERNATE, &d2dGeoms[0], d2dGeoms.size(), &_geometryGroup));
}

void D2DGeometryGroup::addGeometry(Geometry* pGeom)
{
	if (pGeom)
		_geoms.push_back(pGeom);
}

void D2DGeometryGroup::addGeometry(const wstring& geomName)
{
	addGeometry(_renderer->findGeometry(geomName));
}

void D2DGeometryGroup::removeGeometry(Geometry* pGeom)
{
	vector<Geometry*>::iterator it = std::find(_geoms.begin(), _geoms.end(), pGeom);
	if (it != _geoms.end())
		_geoms.erase(it);
}

void D2DGeometryGroup::removeGeometry(const wstring& geomName)
{
	removeGeometry(_renderer->findGeometry(geomName));
}

void D2DGeometryGroup::removeAllGeometries()
{
	_geoms.clear();
}

void D2DGeometryGroup::setGeometries(const vector<Geometry*>& geoms)
{
	_geoms = geoms;
}

ComPtr<ID2D1Geometry> D2DGeometryGroup::getD2DGeometry() const
{
	return _geometryGroup;
}

//////////////////////////////////////////////////////////////////////////
D2DTransformedGeometry::D2DTransformedGeometry(const wstring& name, D2DRenderer* renderer)
	: TransformedGeometry(name)
	, D2DGeometry(renderer)
{
}

bool D2DTransformedGeometry::buildTransformedGeom(Geometry* geom, const Matrix3x3& mat)
{
	D2DGeometry* pD2DGeom = dynamic_cast<D2DGeometry*>(geom);
	if (pD2DGeom)
	{
		D2D1::Matrix3x2F d2dMat = D2D1::Matrix3x2F(mat[0], mat[1], mat[3], mat[4], mat[6], mat[7]);
		return SUCCEEDED(_renderer->getD2DFactory()->CreateTransformedGeometry(pD2DGeom->getD2DGeometry().Get(), d2dMat, &_transformedGeometry));
	}
	return false;
}

ComPtr<ID2D1Geometry> D2DTransformedGeometry::getD2DGeometry() const
{
	return _transformedGeometry;
}

//////////////////////////////////////////////////////////////////////////
D2DPathGeometry::D2DPathGeometry(const wstring& name, D2DRenderer* renderer)
	: PathGeometry(name)
	, D2DGeometry(renderer)
{
}

bool D2DPathGeometry::beginFigure()
{
	if (!_pathGeometry)
	{
		if (FAILED(_renderer->getD2DFactory()->CreatePathGeometry(&_pathGeometry)))
			return false;
	}

	VAYO_ASSERT(!_geometrySink);
	return SUCCEEDED(_pathGeometry->Open(&_geometrySink));
}

void D2DPathGeometry::endFigure()
{
	VAYO_ASSERT(_geometrySink);
	unsigned int ptnum = _path.size();
	if (ptnum > 0)
	{
		_geometrySink->BeginFigure(D2D1::Point2F(_path[0]._x, _path[0]._y), D2D1_FIGURE_BEGIN_FILLED);
		if (ptnum > 1)
		{
			vector<D2D1_POINT_2F> pts;
			pts.resize(ptnum - 1);
			for (unsigned int i = 0; i < ptnum - 1; ++i)
			{
				pts[i].x = _path[i + 1]._x;
				pts[i].y = _path[i + 1]._y;
			}

			_geometrySink->AddLines(&(pts[0]), pts.size());
		}
		_geometrySink->EndFigure(D2D1_FIGURE_END_OPEN);
	}

	_geometrySink->Close();
	_geometrySink.Reset();
}

void D2DPathGeometry::addLine(const Vector2df& pt)
{
	VAYO_ASSERT(_geometrySink);
	_path.push_back(pt);
}

void D2DPathGeometry::addLines(const Vector2df* pt, unsigned int size)
{
	VAYO_ASSERT(_geometrySink);
	for (unsigned int i = 0; i < size; ++i)
		_path.push_back(pt[i]);
}

ComPtr<ID2D1Geometry> D2DPathGeometry::getD2DGeometry() const
{
	return _pathGeometry;
}
