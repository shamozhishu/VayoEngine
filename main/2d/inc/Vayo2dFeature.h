/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 特性
\*************************************************************************/
#ifndef __VAYO2D_FEATURE_H__
#define __VAYO2D_FEATURE_H__

#include "Vayo2dSupport.h"
#include "Vayo2dColor.h"
#include "Vayo2dSurface.h"
#include "VayoSharedPtr.h"
NS_VAYO2D_BEGIN

enum ECapStyle
{
	ECS_FLAT,
	ECS_SQUARE,
	ECS_ROUND,
	ECS_TRIANGLE
};

class Feature
{
public:
	Feature()
		: _clr(1,1,1,1)
		, _capStyle(ECS_FLAT)
		, _strokeWidth(1.0f)
	{}

public:
	Color      _clr;
	ECapStyle  _capStyle;
	float      _strokeWidth;
	SurfacePtr _surface;
};

NS_VAYO2D_END

#endif // __VAYO2D_FEATURE_H__
