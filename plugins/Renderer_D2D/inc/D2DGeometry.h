/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* Direct2D几何形状
\*************************************************************************/
#ifndef __D2D_GEOMETRY_H__
#define __D2D_GEOMETRY_H__

#include "D2DSupport.h"
#include "Vayo2dGeometry.h"

class D2DGeometry : public Geometry
{
public:
	D2DGeometry(const wstring& name, D2DRenderer* renderer);
	~D2DGeometry();
	bool beginFigure();
	void endFigure();
	void addLine(const Vector2df& pt);
	void addLines(const Vector2df* pt, unsigned int size);
	ComPtr<ID2D1PathGeometry> getPathGeometry() const;

private:
	D2DRenderer* _renderer;
	vector<Vector2df> _path;
	ComPtr<ID2D1GeometrySink> _geometrySink;
	ComPtr<ID2D1PathGeometry> _pathGeometry;
};

#endif // __D2D_GEOMETRY_H__
