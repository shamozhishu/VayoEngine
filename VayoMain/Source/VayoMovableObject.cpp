#include "VayoMovableObject.h"
#include "VayoSceneNode.h"
#include "VayoRoot.h"
#include "VayoRenderSystem.h"
#include "VayoMaterialManager.h"
#include "VayoRenderQueue.h"
#include "VayoMesh.h"
#include "VayoUtils.h"

NS_VAYO_BEGIN

const wstring& MovableObject::getName() const
{
	return _name;
}

MovableObject::MovableObject(const wstring& name, SceneManager* originSceneMgr)
	: _name(name)
	, _parentNode(NULL)
	, _visible(true)
	, _queueID(ERQ_MAIN_SOLID)
	, _collideMask(0)
	, _triContainer(NULL)
	, _originSceneMgr(originSceneMgr)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"MovableObject" << idx;
		++idx;
		_name = ss.str();
	}
}

MovableObject::~MovableObject()
{
	SceneNode* parent = _parentNode;
	if (parent)
	{
		parent->detachObject(_name);
	}
}

SceneNode* MovableObject::getParentNode() const
{
	return _parentNode;
}

bool MovableObject::isVisible() const
{
	return _visible;
}

void MovableObject::setVisible(bool visible)
{
	_visible = visible;
}

MeshPtr MovableObject::getMesh() const
{
	return NULL;
}

Aabbox3df MovableObject::getWorldAABB() const
{
	Aabbox3df worldAABB = getLocalAABB();
	if (_parentNode && !worldAABB.isEmpty())
		_parentNode->getAbsTransformation().transformBox(worldAABB);
	return worldAABB;
}

const Aabbox3df& MovableObject::getLocalAABB() const
{
	return _localAABB;
}

void MovableObject::refreshLocalAABB()
{
	Mesh* mesh = getMesh().get();
	if (NULL == mesh)
		_localAABB.setEmpty();
	else
	{
		mesh->recalculateBoundingBox();
		_localAABB = mesh->getBoundingBox();
	}
}

BitState& MovableObject::getCollideMask()
{
	return _collideMask;
}

UserDataBind& MovableObject::getUserDataBind()
{
	return _userDataBind;
}

const UserDataBind& MovableObject::getUserDataBind() const
{
	return _userDataBind;
}

void MovableObject::serialize(XMLElement* outXml)
{
	outXml->SetAttribute("name", unicodeToUtf8(_name).c_str());
	if (!_visible)
		outXml->SetAttribute("visible", _visible);
	if (_queueID != ERQ_MAIN_SOLID)
		outXml->SetAttribute("queueid", _queueID);
	if (!_collideMask.isEmptyState())
		outXml->SetAttribute("collideMask", _collideMask());

	wstringstream ss;
	ss << _userDataBind;
	string strUserData = unicodeToUtf8(ss.str());
	if (!strUserData.empty())
	{
		outXml->LinkEndChild(outXml->GetDocument()->NewText(strUserData.c_str()));
	}
}

bool MovableObject::deserialize(XMLElement* inXml)
{
	if (!inXml)
		return false;
	inXml->QueryBoolAttribute("visible", &_visible);
	inXml->QueryUnsignedAttribute("queueid", &_queueID);
	_collideMask(inXml->UnsignedAttribute("collideMask"));

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
