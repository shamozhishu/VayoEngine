#include "VayoSceneLoader.h"
#include "VayoSceneManager.h"
#include "VayoSceneNode.h"
#include "VayoConfigManager.h"
#include "VayoLog.h"
#include "VayoRoot.h"
#include "VayoUtils.h"
#include "VayoCamera.h"
#include "VayoLight.h"
#include "VayoEntity.h"
#include "VayoManualObject.h"
#include "VayoTextObject.h"

NS_VAYO_BEGIN

MovableObject* SceneLoader::loadObjects(XMLElement* element, const string& typeName)
{
	MovableObject* pObj = NULL;
	IF_CREATE_OBJECT(FPSCamera)
	else IF_CREATE_OBJECT(OrbitCamera)
	else IF_CREATE_OBJECT(EagleEyeCamera)
	else IF_CREATE_OBJECT(DirectionalLight)
	else IF_CREATE_OBJECT(PointLight)
	else IF_CREATE_OBJECT(SpotLight)
	else IF_CREATE_OBJECT(Entity)
	else IF_CREATE_OBJECT(ManualObject)
	else IF_CREATE_OBJECT(TextObject)
	else
	{
		Log::wprint(ELL_WARNING, L"标签[%s]没有对应的类类型实现，无法进行实例化", typeName.c_str());
		return NULL;
	}
	return pObj;
}

NodeAnimator* SceneLoader::loadAnimators(XMLElement* element, const string& typeName)
{
	Log::wprint(ELL_WARNING, L"标签[%s]没有对应的类类型实现，无法进行实例化", typeName.c_str());
	return NULL;
}

SceneLoader::SceneLoader(SceneManager* sceneMgr)
	: _sceneMgr(sceneMgr)
{
}

SceneLoader::~SceneLoader()
{
}

bool SceneLoader::loadScene(const wstring& sceneFile)
{
	if (sceneFile.rfind(L".scene") == wstring::npos)
	{
		Log::wprint(ELL_ERROR, L"场景[%s]文件格式错误", sceneFile.c_str());
		return false;
	}

	const tagSceneConfig& sceneConfig = Root::getSingleton().getConfigManager()->getSceneConfig();
	tinyxml2::XMLDocument doc;
	wstring strTmp = sceneConfig.ScenePath + sceneFile;
	if (doc.LoadFile(w2a_(strTmp).c_str()) != tinyxml2::XML_SUCCESS)
	{
		Log::wprint(ELL_ERROR, L"场景[%s]加载失败", strTmp.c_str());
		return false;
	}

	XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot)
	{
		Log::wprint(ELL_ERROR, L"场景[%s]解析失败", strTmp.c_str());
		return false;
	}

	Root::getSingleton().setCurSceneMgr(_sceneMgr);
	XMLElement* pElem = pRoot->FirstChildElement("SceneNode");
	while (pElem)
	{
		if (!parseSceneRecursion(pElem, NULL))
		{
			Root::getSingleton().setCurSceneMgr(NULL);
			Log::wprint(ELL_ERROR, L"场景[%s]创建失败", strTmp.c_str());
			return false;
		}
		pElem = pElem->NextSiblingElement();
	}

	Log::wprint(ELL_INFORMATION, L"场景[%s]创建成功", strTmp.c_str());
	return true;
}

bool SceneLoader::saveScene(const wstring& sceneFile)
{
	return true;
}

bool SceneLoader::parseSceneRecursion(XMLElement* element, SceneNode* pParent)
{
	string strTmp = element->Value();
	if (strTmp == "SceneNode")
	{
		if (NULL == pParent)
			pParent = _sceneMgr->getRootSceneNode();

		wstring nodeName = utf8ToUnicode(element->Attribute("name"));
		SceneNode* pScnenNode = _sceneMgr->createSceneNode(pParent, nodeName);

		if (NULL == pScnenNode)
		{
			Log::wprint(ELL_WARNING, L"场景节点[%s:%s]创建失败", strTmp.c_str(), nodeName.c_str());
			return false;
		}

		if (!pScnenNode->deserialize(element))
		{
			_sceneMgr->destroySceneNode(pScnenNode);
			Log::wprint(ELL_WARNING, L"场景节点[%s:%s]反序列化失败", strTmp.c_str(), nodeName.c_str());
			return false;
		}

		XMLElement* pElem = element->FirstChildElement();
		while (pElem)
		{
			if (!parseSceneRecursion(pElem, pScnenNode))
				return false;
			pElem = pElem->NextSiblingElement();
		}

		return true;
	}
	else if (strTmp == "Objects")
	{
		MovableObject* pObj = NULL;
		XMLElement* pElem = element->FirstChildElement();
		while (pElem)
		{
			pObj = loadObjects(pElem, pElem->Value());
			if (!pObj)
				return false;
			pParent->attachObject(pObj);
			pElem = pElem->NextSiblingElement();
		}

		return true;
	}
	else if (strTmp == "Animators")
	{
		NodeAnimator* pAnim = NULL;
		XMLElement* pElem = element->FirstChildElement();
		while (pElem)
		{
			pAnim = loadAnimators(pElem, pElem->Value());
			if (!pAnim)
				return false;
			pParent->addAnimator(pAnim);
			pElem = pElem->NextSiblingElement();
		}

		return true;
	}
	
	Log::wprint(ELL_WARNING, L"非法标签[%s]", strTmp.c_str());
	return false;
}

NS_VAYO_END
