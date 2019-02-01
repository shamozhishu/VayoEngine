/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 形状基类
\*************************************************************************/
#ifndef __VAYO2D_SHAPE_H__
#define __VAYO2D_SHAPE_H__

#include "Vayo2dSupport.h"
#include "Vayo2dFeature.h"
#include "math/VayoVector2d.h"
#include "math/VayoMatrix3x3.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Shape
{
public:
	Shape();
	virtual ~Shape();
	virtual void              render() = 0;
	virtual FeaturePtr&       getFeature();
	virtual const FeaturePtr& getFeature() const;
	virtual void              setFeature(const wstring& name);
	virtual void              setFeature(const FeaturePtr& feature);
	virtual void              getWorldTransform(Matrix3x3& mat) const;

protected:
	mutable FeaturePtr _feature;
	PROPERTY_RW(unsigned int, _renderPriority, RenderPriority)
};

NS_VAYO2D_END

#endif // __VAYO2D_SHAPE_H__
