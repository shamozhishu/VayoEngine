/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 可移动的对象基类
\*************************************************************************/
#ifndef __VAYO_MOVABLE_OBJECT_H__
#define __VAYO_MOVABLE_OBJECT_H__

#include "VayoSupport.h"
#include "VayoMaterial.h"
#include "VayoAabbox3d.h"
#include "VayoUserDataBind.h"

namespace tinyxml2
{
	class XMLElement;
}
using namespace tinyxml2;

NS_VAYO_BEGIN

class _VayoExport MovableObject
{
	friend class SceneNode;
	friend class SceneManager;
public:
	MovableObject(const wstring& name);
	virtual ~MovableObject();
	virtual const wstring&   getName() const;
	virtual SceneNode*       getParentNode() const;
	virtual void             update(float dt) {}
	virtual bool             isVisible() const;
	virtual void             setVisible(bool visible);
	virtual const Aabbox3df  getLocalAABB() const;
	virtual Aabbox3df        getWorldAABB() const;
	UserDataBind&            getUserDataBind();
	const UserDataBind&      getUserDataBind() const;

protected:
	virtual bool parseXML(XMLElement* xml) = 0;
	DISALLOW_COPY_AND_ASSIGN(MovableObject)
	PROPERTY_RW(unsigned int, _queueID, QueueID)

protected:
	wstring      _name;
	SceneNode*   _parentNode;
	bool         _visible;
	Aabbox3df    _localAABB;
	UserDataBind _userDataBind;
};

NS_VAYO_END

#endif // __VAYO_MOVABLE_OBJECT_H__
