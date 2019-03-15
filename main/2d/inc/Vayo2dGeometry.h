/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 几何形状
\*************************************************************************/
#ifndef __VAYO2D_GEOMETRY_H__
#define __VAYO2D_GEOMETRY_H__

#include "Vayo2dSupport.h"
#include "VayoVector2d.h"
NS_VAYO2D_BEGIN

class Geometry
{
public:
	Geometry(const wstring& name) : _name(name) {}
	virtual ~Geometry() {}
	virtual bool beginFigure() = 0;
	virtual void endFigure() = 0;
	virtual void addLine(const Vector2df& pt) = 0;
	virtual void addLines(const Vector2df* pt, unsigned int size) = 0;

protected:
	PROPERTY_R_REF(wstring, _name, Name)
};

NS_VAYO2D_END

#endif // __VAYO2D_GEOMETRY_H__
