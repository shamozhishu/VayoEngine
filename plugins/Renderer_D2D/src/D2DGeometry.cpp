#include "D2DGeometry.h"
#include "D2DRenderer.h"

D2DGeometry::D2DGeometry(const wstring& name, D2DRenderer* renderer)
	: Geometry(name)
	, _renderer(renderer)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"Direct2D_Geometry" << idx;
		++idx;
		_name = ss.str();
	}

	_renderer->getD2DFactory()->CreatePathGeometry(&_pathGeometry);
}

D2DGeometry::~D2DGeometry()
{
}

bool D2DGeometry::beginFigure()
{
	VAYO_ASSERT(!_geometrySink);
	return SUCCEEDED(_pathGeometry->Open(&_geometrySink));
}

void D2DGeometry::endFigure()
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

void D2DGeometry::addLine(const Vector2df& pt)
{
	VAYO_ASSERT(_geometrySink);
	_path.push_back(pt);
}

void D2DGeometry::addLines(const Vector2df* pt, unsigned int size)
{
	VAYO_ASSERT(_geometrySink);
	for (unsigned int i = 0; i < size; ++i)
		_path.push_back(pt[i]);
}

ComPtr<ID2D1PathGeometry> D2DGeometry::getPathGeometry() const
{
	return _pathGeometry;
}
