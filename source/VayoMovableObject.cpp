#include "VayoMovableObject.h"
#include "VayoSceneNode.h"
#include "VayoRoot.h"
#include "VayoRenderSystem.h"
#include "VayoMaterialManager.h"
#include "VayoRenderQueue.h"

NS_VAYO_BEGIN

const wstring& MovableObject::getName() const
{
	return _name;
}

MovableObject::MovableObject(const wstring& name)
	: _name(name)
	, _parentNode(NULL)
	, _visible(true)
	, _queueID(ERQ_MAIN_SOLID)
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

const Aabbox3df MovableObject::getLocalAABB() const
{
	return _localAABB;
}

Aabbox3df MovableObject::getWorldAABB() const
{
	Aabbox3df worldAABB = getLocalAABB();
	if (_parentNode && !worldAABB.isEmpty())
		_parentNode->getAbsTransformation().transformBox(worldAABB);
	return worldAABB;
}

UserDataBind& MovableObject::getUserDataBind()
{
	return _userDataBind;
}

const UserDataBind& MovableObject::getUserDataBind() const
{
	return _userDataBind;
}

NS_VAYO_END
