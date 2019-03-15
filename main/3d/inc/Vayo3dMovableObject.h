/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 可移动的对象基类
\*************************************************************************/
#ifndef __VAYO3D_MOVABLE_OBJECT_H__
#define __VAYO3D_MOVABLE_OBJECT_H__

#include "Vayo3dSupport.h"
#include "Vayo3dMaterial.h"
#include "VayoBitState.h"
#include "VayoUserDataBind.h"
#include "VayoAabbox3d.h"
#include "VayoAttribSerializer.h"

NS_VAYO3D_BEGIN

class _Vayo3dExport MovableObject : public AttribSerializer
{
	friend class SceneNode;
public:
	MovableObject(const wstring& name, SceneManager* oriSceneMgr);
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
	PROPERTY_RW(unsigned int, _queueID, QueueID)
	PROPERTY_R(SceneManager*, _oriSceneMgr, OriSceneMgr)
	PROPERTY_RW(TriContainer*, _triContainer, TriContainer)

protected:
	wstring       _name;
	SceneNode*    _parentNode;
	bool          _visible;
	Aabbox3df     _localAABB;
	BitState      _collideMask;
	UserDataBind  _userDataBind;
};

NS_VAYO3D_END

#endif // __VAYO3D_MOVABLE_OBJECT_H__
