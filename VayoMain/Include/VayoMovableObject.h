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
#include "VayoBitState.h"
#include "VayoUserDataBind.h"
#include "VayoAttribSerializer.h"

NS_VAYO_BEGIN

class _VayoExport MovableObject : public AttribSerializer
{
	friend class SceneNode;
public:
	MovableObject(const wstring& name);
	virtual ~MovableObject();
	virtual void             update(float dt) = 0;
	virtual const wstring&   getName() const;
	virtual SceneNode*       getParentNode() const;
	virtual bool             isVisible() const;
	virtual void             setVisible(bool visible);
	virtual MeshPtr          getMesh() const;
	virtual Aabbox3df        getWorldAABB() const;
	virtual const Aabbox3df& getLocalAABB() const;
	virtual void             refreshLocalAABB();
	BitState&                getCollideMask();
	UserDataBind&            getUserDataBind();
	const UserDataBind&      getUserDataBind() const;
	void                     serialize(XMLElement* outXml);
	bool                     deserialize(XMLElement* inXml);

protected:
	DISALLOW_COPY_AND_ASSIGN(MovableObject)
	PROPERTY_RW(unsigned int,  _queueID,      QueueID)
	PROPERTY_RW(TriContainer*, _triContainer, TriContainer)

protected:
	wstring       _name;
	SceneNode*    _parentNode;
	bool          _visible;
	Aabbox3df     _localAABB;
	BitState      _collideMask;
	UserDataBind  _userDataBind;
};

NS_VAYO_END

#endif // __VAYO_MOVABLE_OBJECT_H__
