#include "VayoSceneNode.h"
#include "VayoRoot.h"
#include "VayoUtils.h"
#include "VayoRenderSystem.h"
#include "VayoMovableObject.h"
#include "VayoSceneManager.h"
#include "VayoMaterialManager.h"
#include "VayoWireBoundingBox.h"

NS_VAYO_BEGIN

int SceneNode::getAttachedObjsCount() const
{
	return (int)_objects.size();
}

SceneNode::SceneNode(const wstring& name, Node* parent, SceneManager* mgr)
	: Node(name, parent, mgr)
	, _wireBoundingBox(NULL)
	, _showBoundingBox(false)
	, _isAutomaticCulling(true)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"SceneNode" << idx;
		++idx;
		_name = ss.str();
	}
	_wireBoundingBox = new WireBoundingBox();
}

SceneNode::~SceneNode()
{
	SAFE_DELETE(_wireBoundingBox);
	detachAllObjects();
}

void SceneNode::visit(float dt)
{
	if (isCanVisit())
	{
		updateLocalAABB();
		animating(dt);
		updateWorldAABB();
		if (!_sceneMgr->isCulled(this))
		{
			if (_showBoundingBox)
				_sceneMgr->registerForRendering(_wireBoundingBox, ERQ_WIRE_BOUNDING_BOX);

			MovableObject* pObj;
			map<wstring, MovableObject*>::iterator it = _objects.begin();
			for (; it != _objects.end(); ++it)
			{
				pObj = it->second;
				if (pObj->isVisible())
					pObj->update(dt);
			}
		}
	}
	Node::visit(dt);
}

void SceneNode::showWireBoundingBox(bool bShow)
{
	_showBoundingBox = bShow;
}

bool SceneNode::isShowWireBoundingBox() const
{
	if (NULL == _wireBoundingBox)
		return false;
	return _showBoundingBox;
}

const Aabbox3df& SceneNode::getWorldAABB() const
{
	return _worldAABB;
}

void SceneNode::updateWorldAABB()
{
	_worldAABB.setEmpty();
	map<wstring, MovableObject*>::iterator it = _objects.begin();
	for (; it != _objects.end(); ++it)
	{
		_worldAABB.addInternalBox(it->second->getWorldAABB());
	}
	((WireBoundingBox*)_wireBoundingBox)->setAABB(_worldAABB);
}

void SceneNode::updateLocalAABB()
{
	map<wstring, MovableObject*>::iterator it = _objects.begin();
	for (; it != _objects.end(); ++it)
	{
		it->second->refreshLocalAABB();
	}
}

void SceneNode::attachObject(MovableObject* obj)
{
	if (obj && NULL == getAttacheObject(obj->getName()))
	{
		_objects.insert(make_pair(obj->getName(), obj));
		if (obj->_parentNode)
			obj->_parentNode->detachObject(obj->getName());
		obj->_parentNode = this;
	}
}

MovableObject* SceneNode::getAttacheObject(const wstring& name)
{
	MovableObject* ret = NULL;
	map<wstring, MovableObject*>::iterator it = _objects.find(name);
	if (it != _objects.end())
		ret = it->second;
	return ret;
}

void SceneNode::detachObject(MovableObject* obj)
{
	if (obj)
		detachObject(obj->getName());
}

MovableObject* SceneNode::detachObject(const wstring& name)
{
	MovableObject* ret = NULL;
	map<wstring, MovableObject*>::iterator it = _objects.find(name);
	if (it != _objects.end())
	{
		ret = it->second;
		ret->_parentNode = NULL;
		_objects.erase(it);
	}
	return ret;
}

void SceneNode::detachAllObjects()
{
	map<wstring, MovableObject*>::iterator it = _objects.begin();
	for (; it != _objects.end(); ++it)
	{
		it->second->_parentNode = NULL;
	}
	_objects.clear();
}

SceneNode* SceneNode::createChildSceneNode(const wstring& name /*= L""*/)
{
	SceneNode* pRet = NULL;
	if (_sceneMgr)
		pRet = _sceneMgr->createSceneNode(this, name);
	return pRet;
}

void SceneNode::serialize(XMLElement* outXml)
{

}

bool SceneNode::deserialize(XMLElement* inXml)
{
	if (!inXml)
		return false;

	string strTemp;
	vector<string> container;
	const char* szTmp = inXml->Attribute("relTranslation");
	if (szTmp)
		strTemp = szTmp;
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		_relSpace._translation.set(x, y, z);
	}

	strTemp.clear();
	container.clear();
	szTmp = inXml->Attribute("relRotation");
	if (szTmp)
		strTemp = szTmp;
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		_relSpace._rotation.set(x, y, z);
	}

	strTemp.clear();
	container.clear();
	szTmp = inXml->Attribute("relScale");
	if (szTmp)
		strTemp = szTmp;
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		_relSpace._scale.set(x, y, z);
	}

	szTmp = inXml->Attribute("canVisit");
	if (szTmp)
		strTemp = szTmp;
	_canVisit = (strTemp == "false" ? false : true);

	return true;
}

NS_VAYO_END
