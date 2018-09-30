/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 可渲染对象
\*************************************************************************/
#ifndef __VAYO_RENDERABLE_H__
#define __VAYO_RENDERABLE_H__

#include "VayoMaterial.h"
#include "VayoAabbox3d.h"
#include "VayoMatrix4x4.h"
NS_VAYO_BEGIN

class _VayoExport Renderable
{
public:
	Renderable();
	virtual ~Renderable() {}
	virtual void               render() = 0;
	virtual MaterialPtr&       getMaterial();
	virtual const MaterialPtr& getMaterial() const;
	virtual void               setMaterial(const wstring& name);
	virtual void               setMaterial(const MaterialPtr& material);
	virtual void               getWorldTransform(Matrix4x4& mat) const;

protected:
	mutable MaterialPtr _material;
	PROPERTY_RW(unsigned int, _renderPriority, RenderPriority)
};

NS_VAYO_END

#endif // __VAYO_RENDERABLE_H__
