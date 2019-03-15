/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 树形节点
\*************************************************************************/
#ifndef __VAYO3D_NODE_H__
#define __VAYO3D_NODE_H__

#include "Vayo3dSupport.h"
#include "VayoUserDataBind.h"
#include "VayoMatrix4x4.h"
#include "VayoAttribSerializer.h"

NS_VAYO3D_BEGIN

typedef struct tagSpatialInfo
{
	Vector3df _translation;
	Vector3df _rotation;
	Vector3df _scale;
	tagSpatialInfo() : _scale(1,1,1) {}
} SpatialInfo;

class _Vayo3dExport Node : public AttribSerializer
{
public:
	Node(const wstring& name, Node* parent, SceneManager* oriSceneMgr);
	virtual ~Node();
	virtual void                visit(float dt);
	virtual Matrix4x4           getRelTransform() const;
	virtual bool                isCanVisit();
	virtual void                setCanVisit(bool visit);
	virtual const std::wstring& getName() const;
	virtual void                addChild(Node* child);
	virtual bool                removeChild(Node* child);
	virtual void                removeChildren();
	virtual void                remove();
	virtual void                setParent(Node* newParent);
	virtual Node*               getParent() const;
	virtual const Vector3df&    getScale() const;
	virtual void                setScale(const Vector3df& scale);
	virtual const Vector3df&    getRotation() const;
	virtual void                setRotation(const Vector3df& rotation);
	virtual const Vector3df&    getPosition() const;
	virtual void                setPosition(const Vector3df& newpos);
	virtual Vector3df           getAbsPosition() const;
	virtual void                animating(float dt);
	virtual void                addAnimator(NodeAnimator* animator);
	virtual bool                removeAnimator(NodeAnimator* animator);
	virtual void                removeAnimators();
	
	const list<Node*>&          getChildren() const { return _children; }
	const list<NodeAnimator*>&  getAnimators() const { return _animators; }
	const Matrix4x4&    getAbsTransform() const { return _absTransform; }
	const UserDataBind& getUserDataBind() const { return _userDataBind; }
	UserDataBind&       getUserDataBind() { return _userDataBind; }

protected:
	virtual void updateAbsTransform();
	DISALLOW_COPY_AND_ASSIGN(Node)
	PROPERTY_R(SceneManager*, _oriSceneMgr, OriSceneMgr)
protected:
	wstring               _name;
	bool                  _canVisit;
	SpatialInfo           _relSpace;
	Node*                 _parent;
	list<Node*>           _children;
	list<NodeAnimator*>   _animators;
	Matrix4x4             _absTransform;
	UserDataBind          _userDataBind;
};

NS_VAYO3D_END

#endif // __VAYO3D_NODE_H__
