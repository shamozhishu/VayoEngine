/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 线框包围矩形
\*************************************************************************/
#ifndef __VAYO2D_WIRE_BOUNDING_RECT_H__
#define __VAYO2D_WIRE_BOUNDING_RECT_H__

#include "Vayo2dPivot.h"
#include "Vayo2dGraphics.h"
#include "VayoRectangle.h"
#include "Vayo2dRenderer.h"
#include "Vayo2dFeatureManager.h"
NS_VAYO2D_BEGIN

class WireBoundingRect : public Graphics
{
public:
	WireBoundingRect()
	{
		getFeature()->_fill = false;
		getFeature()->_color = Color(0, 1, 0);
	}

	void render()
	{
		if (_rect.isValid())
		{
			Pivot::getSingleton().getActiveRenderer()->setFeature(*getFeature());
			Pivot::getSingleton().getActiveRenderer()->drawRect(_rect);
		}
	}

	void setRect(const Rectf& rc)
	{
		_rect = rc;
	}

private:
	Rectf _rect;
};

NS_VAYO2D_END

#endif // __VAYO2D_WIRE_BOUNDING_RECT_H__
