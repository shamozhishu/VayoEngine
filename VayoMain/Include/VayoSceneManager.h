/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 场景管理器
\*************************************************************************/
#ifndef __VAYO_SCENE_MANAGER_H__
#define __VAYO_SCENE_MANAGER_H__

#include "VayoRenderQueue.h"
#include "VayoAttribSerializer.h"
NS_VAYO_BEGIN

class _VayoExport SceneManager
{
public:
	SceneManager(const wstring& sceneName);
	~SceneManager();
	void updateRenderQueue();
	void setActiveCamera(Camera* pActiveCamera);
	bool isCulled(SceneNode* sceneNode) const;
	bool registerForRendering(Renderable* pRend, unsigned int queueID = ERQ_MAIN_SOLID);
	void showAllWireBoundingBoxes(bool bShow);

	bool loadScene(const wstring& sceneFile);
	bool saveScene(const wstring& sceneFile);

	template<typename T> T* createSceneNode(SceneNode* parent, const wstring& name = L"");
	template<typename T> T* findSceneNode(const wstring& name);
	void                    destroySceneNode(const wstring& name);
	void                    destroySceneNode(SceneNode* sn);
	void                    destroyAllSceneNodes();

	template<typename T> T* createObject(const wstring& name = L"");
	template<typename T> T* findObject(const wstring& name);
	void                    destroyObject(const wstring& name);
	void                    destroyObject(MovableObject* obj);
	void                    destroyAllObjects();

	template<typename T> T* createAnimator(const wstring& name = L"");
	template<typename T> T* findAnimator(const wstring& name);
	void                    destroyAnimator(const wstring& name);
	void                    destroyAnimator(NodeAnimator* anim);
	void                    destroyAllAnimators();

private:
	bool recursionLoading(XMLElement* element, SceneNode* parent);
	void recursionSaving(XMLElement* element, SceneNode* parent, tinyxml2::XMLDocument& document);
	PROPERTY_R(wstring, _name, Name)
	PROPERTY_R(Camera*, _activeCamera, ActiveCamera)
	PROPERTY_R(SceneNode*, _rootSceneNode, RootSceneNode)
	PROPERTY_R(CollisionDetector*, _collDetector, CollDetector)
	DISALLOW_COPY_AND_ASSIGN(SceneManager)

private:
	RenderQueueGroup             _renderQueues;
	map<wstring, SceneNode*>     _sceneNodesPool;
	map<wstring, MovableObject*> _objectsPool;
	map<wstring, NodeAnimator*>  _animatorsPool;
};

template<typename T>
T* SceneManager::createSceneNode(SceneNode* parent, const wstring& name /*= L""*/)
{
	T* ret = new T(name, parent, this);
	SceneNode* pSceneNode = dynamic_cast<SceneNode*>(ret);
	if (NULL == pSceneNode)
	{
		SAFE_DELETE(ret);
		return NULL;
	}

	SceneNode* pFindedSceneNode = findSceneNode<SceneNode>(pSceneNode->getName());
	if (pFindedSceneNode)
	{
		SAFE_DELETE(pFindedSceneNode);
	}

	_sceneNodesPool[pSceneNode->getName()] = pSceneNode;
	return ret;
}

template<typename T>
T* SceneManager::findSceneNode(const wstring& name)
{
	T* ret = NULL;
	map<wstring, SceneNode*>::iterator it = _sceneNodesPool.find(name);
	if (it != _sceneNodesPool.end())
		ret = dynamic_cast<T*>(it->second);
	return ret;
}

template<typename T>
T* SceneManager::createObject(const wstring& name /*= L""*/)
{
	T* ret = new T(name);
	MovableObject* pObj = dynamic_cast<MovableObject*>(ret);
	if (NULL == pObj)
	{
		SAFE_DELETE(ret);
		return NULL;
	}

	MovableObject* pFindedObj = findObject<MovableObject>(pObj->getName());
	if (pFindedObj)
	{
		SAFE_DELETE(pFindedObj);
	}

	_objectsPool[pObj->getName()] = pObj;
	return ret;
}

template<typename T>
T* SceneManager::findObject(const wstring& name)
{
	T* ret = NULL;
	map<wstring, MovableObject*>::iterator it = _objectsPool.find(name);
	if (it != _objectsPool.end())
		ret = dynamic_cast<T*>(it->second);
	return ret;
}

template<typename T>
T* SceneManager::createAnimator(const wstring& name /*= L""*/)
{
	T* ret = new T(name);
	NodeAnimator* pAnim = dynamic_cast<NodeAnimator*>(ret);
	if (NULL == pAnim)
	{
		SAFE_DELETE(ret);
		return NULL;
	}

	NodeAnimator* pFindedAnim = findAnimator<NodeAnimator>(pAnim->getName());
	if (pFindedAnim)
	{
		SAFE_DELETE(pFindedAnim);
	}

	_animatorsPool[pAnim->getName()] = pAnim;
	return ret;
}

template<typename T>
T* SceneManager::findAnimator(const wstring& name)
{
	T* ret = NULL;
	map<wstring, NodeAnimator*>::iterator it = _animatorsPool.find(name);
	if (it != _animatorsPool.end())
		ret = dynamic_cast<T*>(it->second);
	return ret;
}

NS_VAYO_END

#endif // __VAYO_SCENE_MANAGER_H__
