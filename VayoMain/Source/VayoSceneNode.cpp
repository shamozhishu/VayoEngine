#include "VayoSceneNode.h"
#include "VayoRoot.h"
#include "VayoUtils.h"
#include "VayoRenderSystem.h"
#include "VayoMovableObject.h"
#include "VayoSceneManager.h"
#include "VayoMaterialManager.h"
#include "VayoWireBoundingBox.h"

NS_VAYO_BEGIN

Reflex<SceneNode, const wstring&, Node*, SceneManager*> SceneNode::_dynReflex;
SceneNode::SceneNode(const wstring& name, Node* parent, SceneManager* originSceneMgr)
	: Node(name, parent, originSceneMgr)
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
		if (!_originSceneMgr->isCulled(this))
		{
			if (_showBoundingBox)
				_originSceneMgr->registerForRendering(_wireBoundingBox, ERQ_WIRE_BOUNDING_BOX);

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
	if (!obj || obj->getOriginSceneMgr() != getOriginSceneMgr())
		return;
	if (NULL == getAttacheObject(obj->getName()))
	{
		_objects.insert(make_pair(obj->getName(), obj));
		if (obj->_parentNode)
			obj->_parentNode->detachObject(obj->getName());
		obj->_parentNode = this;
	}
}

int SceneNode::getAttachedObjsCount() const
{
	return (int)_objects.size();
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
	if (_originSceneMgr)
		pRet = _originSceneMgr->createSceneNode<SceneNode>(this, name);
	return pRet;
}

void SceneNode::serialize(XMLElement* outXml)
{
	char szbuf[256];
	outXml->SetAttribute("name", unicodeToUtf8(_name).c_str());

	if (_relSpace._translation != Vector3df::Origin)
	{
		sprintf_s(szbuf, sizeof(szbuf), "%f,%f,%f", _relSpace._translation._x, _relSpace._translation._y, _relSpace._translation._z);
		outXml->SetAttribute("translation", szbuf);
	}

	if (_relSpace._rotation != Vector3df::Origin)
	{
		sprintf_s(szbuf, sizeof(szbuf), "%f,%f,%f", _relSpace._rotation._x, _relSpace._rotation._y, _relSpace._rotation._z);
		outXml->SetAttribute("rotation", szbuf);
	}
	
	if (_relSpace._scale != Vector3df(1, 1, 1))
	{
		sprintf_s(szbuf, sizeof(szbuf), "%f,%f,%f", _relSpace._scale._x, _relSpace._scale._y, _relSpace._scale._z);
		outXml->SetAttribute("scale", szbuf);
	}

	if (!_canVisit)
		outXml->SetAttribute("visit", _canVisit);

	if (_showBoundingBox)
		outXml->SetAttribute("show_boundingbox", _showBoundingBox);

	if (!_isAutomaticCulling)
		outXml->SetAttribute("auto_culling", _isAutomaticCulling);

	wstringstream ss;
	ss << _userDataBind;
	string strUserData = unicodeToUtf8(ss.str());
	if (!strUserData.empty())
	{
		outXml->LinkEndChild(outXml->GetDocument()->NewText(strUserData.c_str()));
	}
}

bool SceneNode::deserialize(XMLElement* inXml)
{
	if (!inXml)
		return false;

	string strTemp;
	vector<string> container;
	const char* szTmp = inXml->Attribute("translation");
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
	szTmp = inXml->Attribute("rotation");
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
	szTmp = inXml->Attribute("scale");
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

	inXml->QueryBoolAttribute("visit", &_canVisit);
	inXml->QueryBoolAttribute("show_boundingbox", &_showBoundingBox);
	inXml->QueryBoolAttribute("auto_culling", &_isAutomaticCulling);

	const char* szUserData = inXml->GetText();
	if (szUserData)
	{
		wstringstream ss;
		ss << utf8ToUnicode(szUserData);
		ss >> _userDataBind;
	}

	return true;
}

NS_VAYO_END
