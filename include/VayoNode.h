/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 树形节点
\*************************************************************************/
#ifndef __VAYO_NODE_H__
#define __VAYO_NODE_H__

#include "VayoSupport.h"
#include "VayoMatrix4x4.h"
#include "VayoUserDataBind.h"

namespace tinyxml2
{
	class XMLElement;
}
using namespace tinyxml2;

NS_VAYO_BEGIN

typedef struct tagSpatialInfo
{
	Vector3df _translation;
	Vector3df _rotation;
	Vector3df _scale;
} SpatialInfo;

class _VayoExport Node
{
	friend class SceneManager;
public:
	Node(const wstring& name, Node* parent, SceneManager* mgr);
	virtual ~Node();
	virtual void                visit(float dt);
	virtual Matrix4x4           getRelTransformation() const;
	virtual bool                isCanVisit();
	virtual void                setCanVisit(bool visible);
	virtual const std::wstring& getName() const;
	virtual void                setName(wstring name);
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
	const Matrix4x4&    getAbsTransformation() const { return _absTransformation; }
	const UserDataBind& getUserDataBind() const { return _userDataBind; }
	UserDataBind&       getUserDataBind() { return _userDataBind; }

protected:
	virtual void updateAbsPosition();
	virtual bool parseXML(XMLElement* xml);
	DISALLOW_COPY_AND_ASSIGN(Node)
protected:
	wstring               _name;
	bool                  _canVisit;
	Matrix4x4             _absTransformation;
	SpatialInfo           _relSpace;
	Node*                 _parent;
	list<Node*>           _children;
	list<NodeAnimator*>   _animators;
	UserDataBind          _userDataBind;
	SceneManager*         _sceneMgr;
};

NS_VAYO_END

#endif // __VAYO_NODE_H__
