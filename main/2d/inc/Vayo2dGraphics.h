/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* ͼ�λ���
\*************************************************************************/
#ifndef __VAYO2D_GRAPHICS_H__
#define __VAYO2D_GRAPHICS_H__

#include "Vayo2dSupport.h"
#include "Vayo2dFeature.h"
#include "math/VayoVector2d.h"
#include "math/VayoMatrix3x3.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Graphics
{
public:
	Graphics();
	virtual ~Graphics() {}
	virtual void              render() = 0;
	virtual FeaturePtr&       getFeature();
	virtual const FeaturePtr& getFeature() const;
	virtual void              setFeature(const wstring& name);
	virtual void              setFeature(const FeaturePtr& feature);
	virtual void              getWorldTransform(Matrix3x3& mat) const;

protected:
	mutable FeaturePtr _feature;
	PROPERTY_RW(unsigned int, _zorder, ZOrder)
};

NS_VAYO2D_END

#endif // __VAYO2D_GRAPHICS_H__