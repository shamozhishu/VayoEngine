/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 场景节点
\*************************************************************************/
#ifndef __VAYO_SCENE_NODE_H__
#define __VAYO_SCENE_NODE_H__

#include "VayoSupport.h"
#include "VayoNode.h"
NS_VAYO_BEGIN

/**
 * Serialized template:
 * <SceneNode name="灯光场景节点" relTranslation="0,0,0" relRotation="0,0,0" relScale="1,1,1" canVisit="true">
**/
class _VayoExport SceneNode : public Node
{
public:
	SceneNode(const wstring& name, Node* parent, SceneManager* mgr);
	virtual ~SceneNode();
	virtual void             visit(float dt);
	virtual void             showWireBoundingBox(bool bShow);
	virtual bool             isShowWireBoundingBox() const;
	virtual const Aabbox3df& getWorldAABB() const;
	virtual void             attachObject(MovableObject* obj);
	int                      getAttachedObjsCount() const;
	MovableObject*           getAttacheObject(const wstring& name);
	virtual void             detachObject(MovableObject* ent);
	virtual MovableObject*   detachObject(const wstring& name);
	virtual void             detachAllObjects();
	virtual SceneNode*       createChildSceneNode(const wstring& name = L"");

	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);
	bool isAutomaticCulling() const { return _isAutomaticCulling; }
	void setAutomaticCulling(bool isCulling) { _isAutomaticCulling = isCulling; }
	const map<wstring, MovableObject*>& getObjects() { return _objects; }

private:
	void updateWorldAABB();
	void updateLocalAABB();

private:
	Aabbox3df                    _worldAABB;
	Renderable*                  _wireBoundingBox;
	map<wstring, MovableObject*> _objects;
	bool                         _showBoundingBox;
	bool                         _isAutomaticCulling;
};

NS_VAYO_END

#endif // __VAYO_SCENE_NODE_H__
