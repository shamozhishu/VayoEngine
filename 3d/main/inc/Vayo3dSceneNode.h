/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 场景节点
\*************************************************************************/
#ifndef __VAYO3D_SCENE_NODE_H__
#define __VAYO3D_SCENE_NODE_H__

#include "Vayo3dSupport.h"
#include "Vayo3dNode.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport SceneNode : public Node
{
	static Reflex<SceneNode, const wstring&, Node*, SceneManager*> _dynReflex;
public:
	SceneNode(const wstring& name, Node* parent, SceneManager* originSceneMgr);
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

NS_VAYO3D_END

#endif // __VAYO3D_SCENE_NODE_H__
