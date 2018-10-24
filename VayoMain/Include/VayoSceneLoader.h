/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 场景加载器
\*************************************************************************/
#ifndef __VAYO_SCENE_LOADER_H__
#define __VAYO_SCENE_LOADER_H__

#include "VayoSupport.h"
#include "VayoAttribSerializer.h"

#define IF_CREATE_OBJECT(OBJ_TYPE)                                                                       \
if (typeName == #OBJ_TYPE)                                                                               \
{                                                                                                        \
	wstring objName = utf8ToUnicode(element->Attribute("name"));                                         \
	pObj = _sceneMgr->findObject<OBJ_TYPE>(objName);                                                     \
	if (NULL == pObj)                                                                                    \
	{                                                                                                    \
		pObj = _sceneMgr->createObject<OBJ_TYPE>(objName);                                               \
		if (NULL == pObj)                                                                                \
		{                                                                                                \
			Log::wprint(ELL_WARNING, L"对象[%s:%s]创建失败", typeName.c_str(), objName.c_str());         \
			return NULL;                                                                                 \
		}                                                                                                \
		if (!pObj->deserialize(element))                                                                 \
		{                                                                                                \
			_sceneMgr->destroyObject(pObj);                                                              \
			Log::wprint(ELL_WARNING, L"对象[%s:%s]反序列化失败", typeName.c_str(), objName.c_str());     \
			return NULL;                                                                                 \
		}                                                                                                \
	}                                                                                                    \
}

#define IF_CREATE_ANIMATOR(ANIM_TYPE)                                                                    \
if (typeName == #ANIM_TYPE)                                                                              \
{                                                                                                        \
	wstring animName = utf8ToUnicode(element->Attribute("name"));                                        \
	pAnim = _sceneMgr->findAnimator<ANIM_TYPE>(animName);                                                \
	if (NULL == pAnim)                                                                                   \
	{                                                                                                    \
		pAnim = _sceneMgr->createAnimator<ANIM_TYPE>(animName);                                          \
		if (NULL == pAnim)                                                                               \
		{                                                                                                \
			Log::wprint(ELL_WARNING, L"节点动画[%s:%s]创建失败", typeName.c_str(), animName.c_str());    \
			return NULL;                                                                                 \
		}                                                                                                \
		if (!pAnim->deserialize(element))                                                                \
		{                                                                                                \
			_sceneMgr->destroyAnimator(pAnim);                                                           \
			Log::wprint(ELL_WARNING, L"节点动画[%s:%s]反序列化失败", typeName.c_str(), animName.c_str());\
			return NULL;                                                                                 \
		}                                                                                                \
	}                                                                                                    \
}

NS_VAYO_BEGIN

class _VayoExport SceneLoader
{
public:
	SceneLoader(SceneManager* sceneMgr);
	virtual ~SceneLoader();
	virtual MovableObject* loadObjects(XMLElement* element, const string& typeName);
	virtual NodeAnimator* loadAnimators(XMLElement* element, const string& typeName);

private:
	friend class SceneManager;
	SceneManager* _sceneMgr;
	bool loadScene(const wstring& sceneFile);
	bool saveScene(const wstring& sceneFile);
	bool parseSceneRecursion(XMLElement* element, SceneNode* pParent);
};

NS_VAYO_END

#endif // __VAYO_SCENE_LOADER_H__
