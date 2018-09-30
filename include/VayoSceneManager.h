/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 场景管理器
\*************************************************************************/
#ifndef __VAYO_SCENE_MANAGER_H__
#define __VAYO_SCENE_MANAGER_H__

#include "VayoSupport.h"
#include "VayoSingleton.h"
#include "VayoRenderable.h"
#include "VayoRenderQueue.h"

namespace tinyxml2
{
	class XMLElement;
}
using namespace tinyxml2;

NS_VAYO_BEGIN

class _VayoExport SceneManager
{
public:
	SceneManager(const wstring& sceneName);
	~SceneManager();
	void updateRenderQueue();
	void setActiveCamera(Camera* pActiveCamera);
	bool isCulled(SceneNode* sceneNode) const;
	bool registerForRendering(Renderable* r, unsigned int q = ERQ_MAIN_SOLID);
	void showAllWireBoundingBoxes(bool bShow);

	SceneNode* findSceneNode(const wstring& name);
	SceneNode* createSceneNode(Node* parent, const wstring& name = L"");
	void       destroySceneNode(SceneNode* sn);
	void       destroySceneNode(const wstring& name);
	void       destroyAllSceneNodes();
	
	template<typename T> T* findObject(const wstring& name);
	template<typename T> T* createObject(const wstring& name = L"");
	void                    destroyObject(MovableObject* obj);
	void                    destroyObject(const wstring& name);
	void                    destroyAllObjects();

private:
	friend class Root;
	bool xmlParseSceneRecursion(XMLElement* xml, SceneNode* pParent);
	PROPERTY_R(wstring,    _name,          Name)
	PROPERTY_R(SceneNode*, _rootSceneNode, RootSceneNode)
	PROPERTY_R(Camera*,    _activeCamera,  ActiveCamera)
	DISALLOW_COPY_AND_ASSIGN(SceneManager)

private:
	RenderQueueGroup                _renderQueues;
	map<wstring, SceneNode*>        _sceneNodesPool;
	map<wstring, MovableObject*>    _objectsPool;
};

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

NS_VAYO_END

#endif // __VAYO_SCENE_MANAGER_H__
