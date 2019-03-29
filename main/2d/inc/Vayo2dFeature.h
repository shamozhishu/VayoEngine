/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 图形特征
\*************************************************************************/
#ifndef __VAYO2D_FEATURE_H__
#define __VAYO2D_FEATURE_H__

#include "Vayo2dSupport.h"
#include "Vayo2dColor.h"
#include "Vayo2dPicture.h"
#include "VayoSharedPtr.h"
NS_VAYO2D_BEGIN

enum ECapStyle
{
	ECS_FLAT,
	ECS_SQUARE,
	ECS_ROUND,
	ECS_TRIANGLE
};

enum ELineJoin
{
	ELJ_MITER,
	ELJ_BEVEL,
	ELJ_ROUND,
	ELJ_MITER_OR_BEVEL
};

enum EDashStyle
{
	EDS_SOLID,
	EDS_DASH,
	EDS_DOT,
	EDS_DASH_DOT,
	EDS_DASH_DOT_DOT,
	EDS_CUSTOM
};

struct StrokeStyleProperties
{
	ECapStyle  startCap = ECS_FLAT;
	ECapStyle  endCap = ECS_FLAT;
	ECapStyle  dashCap = ECS_FLAT;
	ELineJoin  lineJoin = ELJ_MITER;
	float      miterLimit = 10.0f;
	EDashStyle dashStyle = EDS_SOLID;
	float      dashOffset = 0.0f;
	bool operator!=(const StrokeStyleProperties& stroke) const
	{
		return startCap != stroke.startCap ||
			endCap != stroke.endCap ||
			dashCap != stroke.dashCap ||
			lineJoin != stroke.lineJoin ||
			miterLimit != stroke.miterLimit ||
			dashStyle != stroke.dashStyle ||
			dashOffset != stroke.dashOffset;
	}
	bool operator==(const StrokeStyleProperties& stroke) const
	{
		return !(*this != stroke);
	}
};

class Feature
{
public:
	Feature(const wstring& name)
		: _featureName(name)
		, _fill(true)
		, _color(1,1,1,1)
		, _strokeWidth(1.0f) {}

	Feature(const Feature& other)
	{
		*this = other;
	}

	Feature& operator=(const Feature& other)
	{
		if (this == &other)
			return *this;

		_fill = other._fill;
		_color = other._color;
		_strokeWidth = other._strokeWidth;
		_strokeStyle = other._strokeStyle;

		return *this;
	}

	inline bool operator!=(const Feature& b) const
	{
		return _fill != b._fill ||
			_color != b._color ||
			_strokeWidth != b._strokeWidth ||
			_strokeStyle != b._strokeStyle;
	}

	inline bool operator==(const Feature& b) const
	{
		return !(b != *this);
	}

public:
	const wstring  _featureName;
	bool                  _fill;
	Color                 _color;
	PicturePtr            _picture;
	float                 _strokeWidth;
	StrokeStyleProperties _strokeStyle;
};

NS_VAYO2D_END

#endif // __VAYO2D_FEATURE_H__
