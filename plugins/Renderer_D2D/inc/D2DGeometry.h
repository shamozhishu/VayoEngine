/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* Direct2D几何形状
\*************************************************************************/
#ifndef __D2D_GEOMETRY_H__
#define __D2D_GEOMETRY_H__

#include "D2DSupport.h"
#include "Vayo2dGeometry.h"

class D2DGeometry
{
public:
	D2DGeometry(D2DRenderer* renderer);
	virtual ComPtr<ID2D1Geometry> getD2DGeometry() const = 0;

protected:
	D2DRenderer* _renderer;
};

class D2DRectGeometry : public RectGeometry, public D2DGeometry
{
public:
	D2DRectGeometry(const wstring& name, D2DRenderer* renderer);
	bool buildRect(const Rectf& rect);
	ComPtr<ID2D1Geometry> getD2DGeometry() const;

private:
	ComPtr<ID2D1RectangleGeometry> _rectGeometry;
};

class D2DRoundedRectGeometry : public RoundedRectGeometry, public D2DGeometry
{
public:
	D2DRoundedRectGeometry(const wstring& name, D2DRenderer* renderer);
	bool buildRoundedRect(const Rectf& rect, const Vector2df& radius);
	ComPtr<ID2D1Geometry> getD2DGeometry() const;

private:
	ComPtr<ID2D1RoundedRectangleGeometry> _roundedRectGeometry;
};

class D2DEllipseGeometry : public EllipseGeometry, public D2DGeometry
{
public:
	D2DEllipseGeometry(const wstring& name, D2DRenderer* renderer);
	bool buildRound(const Vector2df& center, float radius);
	bool buildEllipse(const Vector2df& center, const Vector2df& radius);
	ComPtr<ID2D1Geometry> getD2DGeometry() const;

private:
	ComPtr<ID2D1EllipseGeometry> _ellipseGeometry;
};

class D2DGeometryGroup : public GeometryGroup, public D2DGeometry
{
public:
	D2DGeometryGroup(const wstring& name, D2DRenderer* renderer);
	bool buildGroup();
	void addGeometry(Geometry* pGeom);
	void addGeometry(const wstring& geomName);
	void removeGeometry(Geometry* pGeom);
	void removeGeometry(const wstring& geomName);
	void removeAllGeometries();
	void setGeometries(const vector<Geometry*>& geoms);
	ComPtr<ID2D1Geometry> getD2DGeometry() const;

private:
	vector<Geometry*> _geoms;
	ComPtr<ID2D1GeometryGroup> _geometryGroup;
};

class D2DTransformedGeometry : public TransformedGeometry, public D2DGeometry
{
public:
	D2DTransformedGeometry(const wstring& name, D2DRenderer* renderer);
	bool buildTransformedGeom(Geometry* geom, const Matrix3x3& mat);
	ComPtr<ID2D1Geometry> getD2DGeometry() const;

private:
	ComPtr<ID2D1TransformedGeometry> _transformedGeometry;
};

class D2DPathGeometry : public PathGeometry, public D2DGeometry
{
public:
	D2DPathGeometry(const wstring& name, D2DRenderer* renderer);
	bool beginFigure();
	void endFigure();
	void addLine(const Vector2df& pt);
	void addLines(const Vector2df* pt, unsigned int size);
	ComPtr<ID2D1Geometry> getD2DGeometry() const;

private:
	vector<Vector2df> _path;
	ComPtr<ID2D1GeometrySink> _geometrySink;
	ComPtr<ID2D1PathGeometry> _pathGeometry;
};

#endif // __D2D_GEOMETRY_H__
