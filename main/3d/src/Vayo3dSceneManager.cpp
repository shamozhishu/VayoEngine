#include "Vayo3dSceneManager.h"
#include "Vayo3dSceneNode.h"
#include "VayoLog.h"
#include "Vayo3dRoot.h"
#include "VayoUtils.h"
#include "Vayo3dCamera.h"
#include "Vayo3dNodeAnimator.h"
#include "Vayo3dCollision.h"
#include "VayoConfigManager.h"
#include "Vayo3dTessGridHandler.h"

NS_VAYO3D_BEGIN

void SceneManager::updateRenderQueue()
{
	_renderQueues.update();
}

SceneManager::SceneManager(const wstring& sceneName)
	: _name(sceneName)
	, _activeCamera(NULL)
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

	_rootSceneNode = new SceneNode(_name + L"_RootSceneNode", NULL, this);
	_collDetector = new CollisionDetector(this);
}

SceneManager::~SceneManager()
{
	SAFE_DELETE(_collDetector);
	destroyAllAnimators();
	destroyAllObjects();
	destroyAllSceneNodes();
	SAFE_DELETE(_rootSceneNode);
}

void SceneManager::setActiveCamera(Camera* pActiveCamera)
{
	if ((pActiveCamera && pActiveCamera->getOriSceneMgr() != this) || pActiveCamera == _activeCamera)
		return;
	if (_activeCamera)
	{
		_activeCamera->enableTouch(false);
		_activeCamera->enableKeypad(false);
	}
	_activeCamera = pActiveCamera;
	if (_activeCamera)
	{
		_activeCamera->enableTouch(true);
		_activeCamera->enableKeypad(true);
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

bool SceneManager::registerForRendering(Renderable* pRend, unsigned int queueID /*= ERQ_MAIN_SOLID*/)
{
	if (NULL == pRend)
		return false;
	_renderQueues.addRenderable(pRend, queueID);
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
	if (sceneFile.rfind(L".scene") == wstring::npos)
	{
		Log::wprint(ELL_WARNING, L"场景[%s]文件格式错误", sceneFile.c_str());
		return false;
	}

	const tagSceneConfig& sceneConfig = ConfigManager::getSingleton().getSceneConfig();
	wstring filename = sceneConfig.ScenePath + sceneFile;
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(w2a_(filename).c_str()) != tinyxml2::XML_SUCCESS)
	{
		Log::wprint(ELL_WARNING, L"场景[%s]加载失败", filename.c_str());
		return false;
	}

	XMLElement* root = doc.RootElement();
	if (NULL == root)
	{
		Log::wprint(ELL_WARNING, L"场景[%s]解析失败", filename.c_str());
		return false;
	}

	_rootSceneNode->deserialize(root);
	destroyAllAnimators();
	destroyAllObjects();
	destroyAllSceneNodes();
	Root::getSingleton().bootFrame(NULL, getName());
	XMLElement* element = root->FirstChildElement();
	while (element)
	{
		if (!recursionLoading(element, _rootSceneNode))
		{
			Root::getSingleton().setNullCurSceneMgr();
			destroyAllAnimators();
			destroyAllObjects();
			destroyAllSceneNodes();
			Log::wprint(ELL_WARNING, L"场景[%s]创建失败", filename.c_str());
			return false;
		}
		element = element->NextSiblingElement();
	}

	do 
	{
		bool parseOK = true;

		if (parseOK)
		{
			const char* szbuf = root->GetText();
			if (szbuf)
			{
				TessGridHandler girdHandler;
				wstring tessgridFiles = utf8ToUnicode(szbuf);
				vector<wstring> container;
				wstringtok(container, tessgridFiles, L",");
				unsigned int size = container.size();
				for (unsigned int i = 0; i < size; ++i)
				{
					if (!girdHandler.parseTessgridFile(container[i]))
					{
						parseOK = false;
						break;
					}
				}
			}
		}

		if (parseOK)
		{
			SceneNode* sceneNode = NULL;
			map<wstring, SceneNode*>::iterator itNode = _sceneNodesPool.begin();
			for (; itNode != _sceneNodesPool.end(); ++itNode)
			{
				sceneNode = itNode->second;
				if (!(sceneNode->parseCustomAttrib()))
				{
					Log::wprint(ELL_WARNING, L"场景节点%s[%s]定制属性解析失败", itNode->first.c_str(), a2w_(sceneNode->getType()).c_str());
					parseOK = false;
					break;
				}
			}
		}

		if (parseOK)
		{
			MovableObject* obj = NULL;
			map<wstring, MovableObject*>::iterator itObj = _objectsPool.begin();
			for (; itObj != _objectsPool.end(); ++itObj)
			{
				obj = itObj->second;
				if (!(obj->parseCustomAttrib()))
				{
					Log::wprint(ELL_WARNING, L"对象%s[%s]定制属性解析失败", itObj->first.c_str(), a2w_(obj->getType()).c_str());
					parseOK = false;
					break;
				}
			}
		}

		if (parseOK)
		{
			NodeAnimator* anim = NULL;
			map<wstring, NodeAnimator*>::iterator itAnim = _animatorsPool.begin();
			for (; itAnim != _animatorsPool.end(); ++itAnim)
			{
				anim = itAnim->second;
				if (!(anim->parseCustomAttrib()))
				{
					Log::wprint(ELL_WARNING, L"节点动画%s[%s]定制属性解析失败", itAnim->first.c_str(), a2w_(anim->getType()).c_str());
					parseOK = false;
					break;
				}
			}
		}

		if (!parseOK)
		{
			Root::getSingleton().setNullCurSceneMgr();
			destroyAllAnimators();
			destroyAllObjects();
			destroyAllSceneNodes();
			return false;
		}

	} while (0);

	Log::wprint(ELL_DEBUG, L"场景[%s]创建成功", filename.c_str());
	return true;
}

bool SceneManager::saveScene(const wstring& sceneFile)
{
	const tagSceneConfig& sceneConfig = ConfigManager::getSingleton().getSceneConfig();
	wstring filename = sceneConfig.ScenePath + sceneFile;
	tinyxml2::XMLDocument doc;
	doc.InsertFirstChild(doc.NewDeclaration());
	XMLElement* root = doc.NewElement("Scene");
	doc.InsertEndChild(root);
	_rootSceneNode->serialize(root);
	recursionSaving(root, _rootSceneNode, doc);

	if (tinyxml2::XML_SUCCESS != doc.SaveFile(w2a_(filename).c_str()))
	{
		Log::wprint(ELL_ERROR, L"场景[%s]保存失败", filename.c_str());
		return false;
	}

	Log::wprint(ELL_DEBUG, L"场景[%s]保存成功", filename.c_str());
	return true;
}

void SceneManager::destroySceneNode(const wstring& name, bool cleanUpChildren /*= false*/)
{
	SceneNode* node = NULL;
	map<wstring, SceneNode*>::iterator it = _sceneNodesPool.find(name);
	if (it != _sceneNodesPool.end())
	{
		node = it->second;
		_sceneNodesPool.erase(it);

		if (cleanUpChildren)
		{
			list<Node*> nodes = node->getChildren();
			list<Node*>::const_iterator cit = nodes.cbegin();
			for (; cit != nodes.cend(); ++cit)
			{
				SceneNode* pChild = dynamic_cast<SceneNode*>(*cit);
				if (pChild)
					destroySceneNode(pChild->getName(), cleanUpChildren);
			}

			map<wstring, MovableObject*> objs = node->getObjects();
			map<wstring, MovableObject*>::const_iterator itmap = objs.cbegin();
			for (; itmap != objs.cend(); ++itmap)
			{
				MovableObject* pObj = itmap->second;
				destroyObject(pObj->getName());
			}
		}

		delete node;
	}
}

void SceneManager::destroySceneNode(SceneNode* sn, bool cleanUpChildren /*= false*/)
{
	if (sn)
		destroySceneNode(sn->getName(), cleanUpChildren);
}

void SceneManager::destroyAllSceneNodes()
{
	SceneNode* node = NULL;
	map<wstring, SceneNode*>::iterator it = _sceneNodesPool.begin();
	for (; it != _sceneNodesPool.end(); ++it)
	{
		node = it->second;
		delete node;
	}
	_sceneNodesPool.clear();
}

void SceneManager::destroyObject(const wstring& name)
{
	MovableObject* obj = NULL;
	map<wstring, MovableObject*>::iterator it = _objectsPool.find(name);
	if (it != _objectsPool.end())
	{
		obj = it->second;
		delete obj;
		_objectsPool.erase(it);
	}
}

void SceneManager::destroyObject(MovableObject* obj)
{
	if (obj)
		destroyObject(obj->getName());
}

void SceneManager::destroyAllObjects()
{
	MovableObject* obj = NULL;
	map<wstring, MovableObject*>::iterator it = _objectsPool.begin();
	for (; it != _objectsPool.end(); ++it)
	{
		obj = it->second;
		delete obj;
	}
	_objectsPool.clear();
}

void SceneManager::destroyAnimator(const wstring& name)
{
	NodeAnimator* anim = NULL;
	map<wstring, NodeAnimator*>::iterator it = _animatorsPool.find(name);
	if (it != _animatorsPool.end())
	{
		anim = it->second;
		delete anim;
		_animatorsPool.erase(it);
	}
}

void SceneManager::destroyAnimator(NodeAnimator* anim)
{
	if (anim)
		destroyAnimator(anim->getName());
}

void SceneManager::destroyAllAnimators()
{
	NodeAnimator* anim = NULL;
	map<wstring, NodeAnimator*>::iterator it = _animatorsPool.begin();
	for (; it != _animatorsPool.end(); ++it)
	{
		anim = it->second;
		delete anim;
	}
	_animatorsPool.clear();
}

bool SceneManager::recursionLoading(XMLElement* element, SceneNode* parent)
{
	if (NULL == parent)
		parent = _rootSceneNode;

	const char* szname = NULL;
	wstring typeTag = utf8ToUnicode(element->Value());
	if (typeTag == L"Objects")
	{
		MovableObject* pObj = NULL;
		XMLElement* child = element->FirstChildElement();
		while (child)
		{
			typeTag = utf8ToUnicode(child->Value());
			szname = child->Attribute("name");
			wstring objName;
			if (szname && 0 != strcmp(szname, ""))
				objName = utf8ToUnicode(szname);

			pObj = findObject<MovableObject>(objName);
			if (pObj)
			{
				Log::wprint(ELL_WARNING, L"对象%s[%s]已存在", objName.c_str(), typeTag.c_str());
				return false;
			}

			pObj = ReflexFactory<const wstring&, SceneManager*>::getInstance().create<MovableObject>(w2a_(typeTag), objName, this);
			if (!pObj)
			{
				Log::wprint(ELL_WARNING, L"非法标签[%s]", typeTag.c_str());
				return false;
			}

			objName = pObj->getName();
			if (!pObj->deserialize(child))
			{
				delete pObj;
				Log::wprint(ELL_WARNING, L"对象%s[%s]反序列化失败", objName.c_str(), typeTag.c_str());
				return false;
			}

			_objectsPool.insert(make_pair(objName, pObj));
			parent->attachObject(pObj);
			child = child->NextSiblingElement();
		}

		return true;
	}
	else if (typeTag == L"Animators")
	{
		NodeAnimator* pAnim = NULL;
		XMLElement* child = element->FirstChildElement();
		while (child)
		{
			typeTag = utf8ToUnicode(child->Value());
			szname = child->Attribute("name");
			wstring animName;
			if (szname && 0 != strcmp(szname, ""))
				animName = utf8ToUnicode(szname);

			pAnim = findAnimator<NodeAnimator>(animName);
			if (!pAnim)
			{
				pAnim = ReflexFactory<const wstring&, SceneManager*>::getInstance().create<NodeAnimator>(w2a_(typeTag), animName, this);
				if (!pAnim)
				{
					Log::wprint(ELL_WARNING, L"非法标签[%s]", typeTag.c_str());
					return false;
				}

				animName = pAnim->getName();
				if (!pAnim->deserialize(child))
				{
					delete pAnim;
					Log::wprint(ELL_WARNING, L"节点动画%s[%s]反序列化失败", animName.c_str(), typeTag.c_str());
					return false;
				}

				_animatorsPool.insert(make_pair(animName, pAnim));
			}

			parent->addAnimator(pAnim);
			child = child->NextSiblingElement();
		}

		return true;
	}
	else
	{
		szname = element->Attribute("name");
		wstring nodeName;
		if (szname && 0 != strcmp(szname, ""))
			nodeName = utf8ToUnicode(szname);

		SceneNode* pScnenNode = findSceneNode<SceneNode>(nodeName);
		if (pScnenNode)
		{
			Log::wprint(ELL_WARNING, L"场景节点%s[%s]已存在", nodeName.c_str(), typeTag.c_str());
			return false;
		}

		pScnenNode = ReflexFactory<const wstring&, Node*, SceneManager*>::getInstance().create<SceneNode>(w2a_(typeTag), nodeName, parent, this);
		if (!pScnenNode)
		{
			Log::wprint(ELL_WARNING, L"非法标签[%s]", typeTag.c_str());
			return false;
		}

		nodeName = pScnenNode->getName();
		if (!pScnenNode->deserialize(element))
		{
			delete pScnenNode;
			Log::wprint(ELL_WARNING, L"场景节点%s[%s]反序列化失败", nodeName.c_str(), typeTag.c_str());
			return false;
		}

		_sceneNodesPool.insert(make_pair(nodeName, pScnenNode));
		XMLElement* child = element->FirstChildElement();
		while (child)
		{
			if (!recursionLoading(child, pScnenNode))
				return false;
			child = child->NextSiblingElement();
		}

		return true;
	}

	Log::wprint(ELL_WARNING, L"非法标签[%s]", typeTag.c_str());
	return false;
}

void SceneManager::recursionSaving(XMLElement* element, SceneNode* parent, tinyxml2::XMLDocument& document)
{
	XMLElement* child = NULL;
	if (NULL == parent)
		parent = _rootSceneNode;

	if (parent->getAttachedObjsCount() > 0)
	{
		XMLElement* objectsElem = document.NewElement("Objects");
		element->InsertEndChild(objectsElem);

		MovableObject* pObj = NULL;
		const map<wstring, MovableObject*>& objects = parent->getObjects();
		map<wstring, MovableObject*>::const_iterator cit = objects.cbegin();
		for (; cit != objects.cend(); ++cit)
		{
			pObj = cit->second;
			child = document.NewElement(pObj->getType().c_str());
			objectsElem->InsertEndChild(child);
			pObj->serialize(child);
		}
	}

	const list<NodeAnimator*>& animators = parent->getAnimators();
	if (animators.size() > 0)
	{
		XMLElement* animatorsElem = document.NewElement("Animators");
		element->InsertEndChild(animatorsElem);

		NodeAnimator* pAnim = NULL;
		list<NodeAnimator*>::const_iterator cit = animators.cbegin();
		for (; cit != animators.cend(); ++cit)
		{
			pAnim = *cit;
			child = document.NewElement(pAnim->getType().c_str());
			animatorsElem->InsertEndChild(child);
			pAnim->serialize(child);
		}
	}

	const list<Node*>& children = parent->getChildren();
	list<Node*>::const_iterator it = children.cbegin();
	for (; it != children.cend(); ++it)
	{
		parent = (SceneNode*)(*it);
		child = document.NewElement(parent->getType().c_str());
		element->InsertEndChild(child);
		parent->serialize(child);
		recursionSaving(child, parent, document);
	}
}

NS_VAYO3D_END
