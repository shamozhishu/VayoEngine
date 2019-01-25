#include "Vayo2dShape.h"

NS_VAYO2D_BEGIN

void Shape::update(float dt)
{
}

Shape::Shape(unsigned int color)
{
}

Shape::~Shape()
{
	safeRelease(_colorBrush);
}

void Shape::setPt(float x, float y, unsigned int idx /*= -1*/)
{
	setPt(Vector2df(x, y), idx);
}

void Shape::setPt(const Vector2df& pt, unsigned int idx /*= -1*/)
{
	unsigned int ptCnt = _points.size();
	if (idx >= ptCnt)
		_points.push_back(pt);
	else
		_points[idx] = pt;
}

void Shape::setPts(const vector<Vector2df>& pts)
{
	_points = pts;
}

void Shape::addPts(const vector<Vector2df>& pts, unsigned int idx /*= -1*/)
{
	vector<Vector2df>::iterator it = _points.end();
	if (idx < _points.size())
		it = _points.begin() + idx;
	_points.insert(it, pts.begin(), pts.end());
}

Vector2df& Shape::getPt(unsigned int idx /*= -1*/)
{
	unsigned int ptCnt = _points.size();
	return _points[(idx >= ptCnt ? ptCnt - 1 : idx)];
}

const Vector2df& Shape::getPt(unsigned int idx /*= -1*/) const
{
	unsigned int ptCnt = _points.size();
	return _points[(idx >= ptCnt ? ptCnt - 1 : idx)];
}

vector<Vector2df>& Shape::getPts()
{
	return _points;
}

const vector<Vector2df>& Shape::getPts() const
{
	return _points;
}

unsigned int Shape::getPtCount() const
{
	return _points.size();
}

void Shape::clearPts()
{
	_points.clear();
}

NS_VAYO2D_END
