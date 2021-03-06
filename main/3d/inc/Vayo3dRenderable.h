/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 可渲染对象
\*************************************************************************/
#ifndef __VAYO3D_RENDERABLE_H__
#define __VAYO3D_RENDERABLE_H__

#include "Vayo3dMaterial.h"
#include "VayoMatrix4x4.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport Renderable
{
public:
	Renderable();
	virtual ~Renderable();
	virtual void        render() = 0;
	virtual MaterialPtr getMaterial() const;
	virtual void        setMaterial(MaterialPtr material) const;
	virtual void        setMaterial(const wstring& name) const;
	virtual void        getWorldTransform(Matrix4x4& mat) const;

private:
	mutable MaterialPtr _material;
	const wstring _selfMaterialName;
	PROPERTY_RW(unsigned int, _renderPriority, RenderPriority)
};

NS_VAYO3D_END

#endif // __VAYO3D_RENDERABLE_H__
