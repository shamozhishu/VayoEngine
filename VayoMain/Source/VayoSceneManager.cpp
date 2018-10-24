#include "VayoSceneManager.h"
#include "VayoSceneNode.h"
#include "VayoRoot.h"
#include "VayoCamera.h"
#include "VayoNodeAnimator.h"
#include "VayoCollision.h"
#include "VayoSceneLoader.h"

NS_VAYO_BEGIN

void SceneManager::setSceneLoader(SceneLoader* sceneLoader)
{
	if (_sceneLoader)
		delete _sceneLoader;
	_sceneLoader = sceneLoader;
}

SceneManager::SceneManager(const wstring& sceneName)
	: _name(sceneName)
	, _activeCamera(NULL)
	, _sceneLoader(NULL)
	, _rootSceneNode(NULL)
	, _collDetector(NULL)
	, _renderQueues(this)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"SceneManager" << idx;
		++idx;
		_name = ss.str();
	}

	_rootSceneNode = createSceneNode(NULL, L"RootSceneNode");
	_sceneLoader = new SceneLoader(this);
	_collDetector = new CollisionDetector(this);
}

SceneManager::~SceneManager()
{
	SAFE_DELETE(_collDetector);
	setSceneLoader(NULL);
	destroyAllAnimators();
	destroyAllObjects();
	destroyAllSceneNodes();
}

void SceneManager::updateRenderQueue()
{
	_renderQueues.update();
}

void SceneManager::setActiveCamera(Camera* pActiveCamera)
{
	if (pActiveCamera == _activeCamera)
		return;
	if (_activeCamera)
	{
		Root::getSingleton().getTouchDispatcher()->removeTouchDelegate(_activeCamera);
		Root::getSingleton().getKeypadDispatcher()->removeKeypadDelegate(_activeCamera);
	}
	_activeCamera = pActiveCamera;
	if (_activeCamera)
	{
		Root::getSingleton().getTouchDispatcher()->addTouchDelegate(_activeCamera);
		Root::getSingleton().getKeypadDispatcher()->addKeypadDelegate(_activeCamera);
		_activeCamera->setNeedUpdate(true);
	}
}

bool SceneManager::isCulled(SceneNode* sceneNode) const
{
	bool result = false;
	if (!_activeCamera || !sceneNode->isAutomaticCulling())
		return result;

	Aabbox3df box = sceneNode->getWorldAABB();
	if (!box.isEmpty())
	{
		Frustum frust = _activeCamera->getViewFrustum();
		Vector3df edges[8];
		box.getEdges(edges);

		for (int i = 0; i < Frustum::EVP_PLANE_COUNT; ++i)
		{
			bool boxInFrustum = false;
			for (unsigned int j = 0; j < 8; ++j)
			{
				if (frust._planes[i].classifyPointRelation(edges[j]) != EIR_FRONT)
				{
					boxInFrustum = true;
					break;
				}
			}

			if (!boxInFrustum)
			{
				result = true;
				break;
			}
		}
	}

	return result;
}

bool SceneManager::registerForRendering(Renderable* r, unsigned int q /*= ERQ_MAIN_SOLID*/)
{
	if (NULL == r)
		return false;
	_renderQueues.addRenderable(r, q);
	return true;
}

void SceneManager::showAllWireBoundingBoxes(bool bShow)
{
	map<wstring, SceneNode*>::iterator it = _sceneNodesPool.begin();
	for (; it != _sceneNodesPool.end(); ++it)
		it->second->showWireBoundingBox(bShow);
}

bool SceneManager::loadScene(const wstring& sceneFile)
{
	if (_sceneLoader)
		return _sceneLoader->loadScene(sceneFile);
	return false;
}

bool SceneManager::saveScene(const wstring& sceneFile)
{
	if (_sceneLoader)
		return _sceneLoader->saveScene(sceneFile);
	return false;
}

SceneNode* SceneManager::createSceneNode(Node* parent, const wstring& name /*= L""*/)
{
	SceneNode* pSceneNode = new SceneNode(name, parent, this);
	if (!pSceneNode)
		return NULL;

	SceneNode* pFindedSceneNode = findSceneNode(pSceneNode->getName());
	if (pFindedSceneNode)
	{
		SAFE_DELETE(pFindedSceneNode);
	}

	_sceneNodesPool[pSceneNode->getName()] = pSceneNode;
	return pSceneNode;
}

SceneNode* SceneManager::findSceneNode(const wstring& name)
{
	SceneNode* ret = NULL;
	map<wstring, SceneNode*>::iterator it = _sceneNodesPool.find(name);
	if (it != _sceneNodesPool.end())
		ret = it->second;
	return ret;
}

void SceneManager::destroySceneNode(SceneNode* sn)
{
	if (sn)
		destroySceneNode(sn->getName());
}

void SceneManager::destroySceneNode(const wstring& name)
{
	map<wstring, SceneNode*>::iterator it = _sceneNodesPool.find(name);
	if (it != _sceneNodesPool.end())
	{
		delete it->second;
		_sceneNodesPool.erase(it);
	}
}

void SceneManager::destroyAllSceneNodes()
{
	map<wstring, SceneNode*>::iterator it = _sceneNodesPool.begin();
	for (; it != _sceneNodesPool.end(); ++it)
		delete it->second;
	_sceneNodesPool.clear();
}

void SceneManager::destroyObject(MovableObject* obj)
{
	if (obj)
		destroyObject(obj->getName());
}

void SceneManager::destroyObject(const wstring& name)
{
	map<wstring, MovableObject*>::iterator it = _objectsPool.find(name);
	if (it != _objectsPool.end())
	{
		delete it->second;
		_objectsPool.erase(it);
	}
}

void SceneManager::destroyAllObjects()
{
	map<wstring, MovableObject*>::iterator it = _objectsPool.begin();
	for (; it != _objectsPool.end(); ++it)
		delete it->second;
	_objectsPool.clear();
}

void SceneManager::destroyAnimator(NodeAnimator* anim)
{
	if (anim)
		destroyAnimator(anim->getName());
}

void SceneManager::destroyAnimator(const wstring& name)
{
	map<wstring, NodeAnimator*>::iterator it = _animatorsPool.find(name);
	if (it != _animatorsPool.end())
	{
		delete it->second;
		_animatorsPool.erase(it);
	}
}

void SceneManager::destroyAllAnimators()
{
	map<wstring, NodeAnimator*>::iterator it = _animatorsPool.begin();
	for (; it != _animatorsPool.end(); ++it)
		delete it->second;
	_animatorsPool.clear();
}

NS_VAYO_END
