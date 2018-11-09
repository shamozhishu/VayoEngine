/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 线框包围盒
\*************************************************************************/
#ifndef __VAYO_WIRE_BOUNDING_BOX_H__
#define __VAYO_WIRE_BOUNDING_BOX_H__

#include "VayoRoot.h"
#include "VayoAabbox3d.h"
#include "VayoRenderable.h"
#include "VayoRenderSystem.h"
#include "VayoMaterialManager.h"
NS_VAYO_BEGIN

class WireBoundingBox : public Renderable
{
public:
	WireBoundingBox() {
		_material = Root::getSingleton().getMaterialManager()->createMaterial(L"Material_WireBoundingBox");
		_material->_lighting = false;
	}

	void render() {
		if (!_aabb.isEmpty())
		{
			Root::getSingleton().getActiveRenderer()->setMaterial(*_material);
			Root::getSingleton().getActiveRenderer()->draw3DBox(_aabb, Colour(255, 0, 255, 0));
		}
	}

	void setAABB(const Aabbox3df& aabb) {
		_aabb = aabb;
	}

private:
	Aabbox3df _aabb;
};

NS_VAYO_END

#endif // __VAYO_WIRE_BOUNDING_BOX_H__
