/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 表面
\*************************************************************************/
#ifndef __VAYO2D_SURFACE_H__
#define __VAYO2D_SURFACE_H__

#include "Vayo2dSupport.h"
#include "math/VayoDimension2d.h"
NS_VAYO2D_BEGIN

class Surface
{
public:
	virtual ~Surface() {}

protected:
	PROPERTY_RW_REF(Dimension2df, _size, Size)
};

NS_VAYO2D_END

#endif // __VAYO2D_SURFACE_H__
