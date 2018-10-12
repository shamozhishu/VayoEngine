#include "VayoSceneManager.h"
#include "tinyxml2/tinyxml2.h"
#include "VayoSceneNode.h"
#include "VayoRenderSystem.h"
#include "VayoUtils.h"
#include "VayoCamera.h"
#include "VayoLight.h"
#include "VayoManualObject.h"
#include "VayoEntity.h"
#include "VayoRoot.h"
#include "VayoNodeAnimator.h"
#include "VayoCollision.h"

#define ELSE_IF_CREATE_OBJECT(OBJ_TYPE)                                            \
else if (strTmp == #OBJ_TYPE)                                                      \
{                                                                                  \
	wstring objName = utf8ToUnicode(xml->Attribute("name"));                       \
	pObj = findObject<OBJ_TYPE>(objName);                                          \
	if (NULL == pObj)                                                              \
	{                                                                              \
		pObj = createObject<OBJ_TYPE>(objName);                                    \
		if (NULL == pObj || !pObj->parseXML(xml))                                  \
		{                                                                          \
			return false;                                                          \
		}                                                                          \
	}                                                                              \
}

NS_VAYO_BEGIN

bool SceneManager::xmlParseSceneRecursion(XMLElement* xml, SceneNode* pParent)
{
	MovableObject* pObj = NULL;
	string strTmp = xml->Value();
	/*<SceneNode name="灯光场景节点" relTranslation="0,0,0" relRotation="0,0,0" relScale="1,1,1" isVisible="true">*/
	if (strTmp == "SceneNode")
	{
		if (NULL == pParent)
			pParent = _rootSceneNode;

		SceneNode* pScnenNode = createSceneNode(pParent, utf8ToUnicode(xml->Attribute("name")));

		if (NULL == pScnenNode || !pScnenNode->parseXML(xml))
			return false;

		XMLElement* pElem = xml->FirstChildElement();
		while (pElem)
		{
			if (!xmlParseSceneRecursion(pElem, pScnenNode))
				return false;

			pElem = pElem->NextSiblingElement();
		}

		return true;
	}
	/*<FPSCamera name="第一人称摄像机" position="0,100,100" target="0,0,0" worldUp="0,1,0" fovY="45" nearZ="1" farZ="1000" moveSpeed="50"/>*/
	ELSE_IF_CREATE_OBJECT(FPSCamera)
	/*<OrbitCamera name="轨道摄像机" position="0,0,100" target="0,0,0" worldUp="0,1,0" fovY="45" nearZ="1" farZ="1000" moveSpeed="20" zoomSpeed="20"/>*/
	ELSE_IF_CREATE_OBJECT(OrbitCamera)
	/*<DirectionalLight name="主方向光" ambientColor="0xff808080" diffuseColor="0xffffffff" specularColor="ffcccccc" direction="-0.57735,0.57735,0.57735"/>*/
	ELSE_IF_CREATE_OBJECT(DirectionalLight)
	ELSE_IF_CREATE_OBJECT(PointLight)
	ELSE_IF_CREATE_OBJECT(SpotLight)
	/*<ManualObject name="球" material="examples/sphere" custom="Sphere,5,100,100"/>*/
	ELSE_IF_CREATE_OBJECT(ManualObject)
	/*<Entity name="立方体" mesh="Cube,examples/cube,10,10,10"/>*/
	ELSE_IF_CREATE_OBJECT(Entity)
	pParent->attachObject(pObj);
	return true;
}

SceneManager::SceneManager(const wstring& sceneName)
	: _name(sceneName)
	, _activeCamera(NULL)
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
	_collDetector = new CollisionDetector(this);
}

SceneManager::~SceneManager()
{
	SAFE_DELETE(_collDetector);
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

SceneNode* SceneManager::findSceneNode(const wstring& name)
{
	SceneNode* ret = NULL;
	map<wstring, SceneNode*>::iterator it = _sceneNodesPool.find(name);
	if (it != _sceneNodesPool.end())
		ret = it->second;
	return ret;
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
