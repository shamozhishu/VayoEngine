/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 图形基类
\*************************************************************************/
#ifndef __VAYO2D_GRAPHICS_H__
#define __VAYO2D_GRAPHICS_H__

#include "Vayo2dSupport.h"
#include "Vayo2dFeature.h"
#include "VayoVector2d.h"
#include "VayoMatrix3x3.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Graphics
{
public:
	Graphics();
	virtual ~Graphics() {}
	virtual void       render() = 0;
	virtual FeaturePtr getFeature() const;
	virtual void       setFeature(FeaturePtr feature);
	virtual void       setFeature(const wstring& name);
	virtual void       getWorldTransform(Matrix3x3& mat) const;

protected:
	mutable FeaturePtr _feature;
	PROPERTY_RW(unsigned int, _zorder, ZOrder)
};

NS_VAYO2D_END

#endif // __VAYO2D_GRAPHICS_H__
