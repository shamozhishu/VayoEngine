/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 线框包围盒
\*************************************************************************/
#ifndef __VAYO3D_WIRE_BOUNDING_BOX_H__
#define __VAYO3D_WIRE_BOUNDING_BOX_H__

#include "Vayo3dRoot.h"
#include "Vayo3dRenderable.h"
#include "VayoAabbox3d.h"
#include "Vayo3dRenderSystem.h"
#include "Vayo3dMaterialManager.h"
NS_VAYO3D_BEGIN

class WireBoundingBox : public Renderable
{
public:
	WireBoundingBox()
	{
		getMaterial()->_lighting = false;
	}

	void render()
	{
		if (!_aabb.isEmpty())
		{
			Root::getSingleton().getActiveRenderer()->setMaterial(*getMaterial());
			Root::getSingleton().getActiveRenderer()->draw3DBox(_aabb, Colour(255, 0, 255, 0));
		}
	}

	void setAABB(const Aabbox3df& aabb)
	{
		_aabb = aabb;
	}

private:
	Aabbox3df _aabb;
};

NS_VAYO3D_END

#endif // __VAYO3D_WIRE_BOUNDING_BOX_H__
