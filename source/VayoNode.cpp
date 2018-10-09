#include "VayoNode.h"
#include "VayoEntity.h"
#include "VayoMath.h"
#include "tinyxml2/tinyxml2.h"
#include "VayoUtils.h"
#include "VayoNodeAnimator.h"

NS_VAYO_BEGIN

bool Node::parseXML(XMLElement* xml)
{
	if (!xml)
		return false;

	string strTemp;
	vector<string> container;
	const char* szTmp = xml->Attribute("relTranslation");
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
	szTmp = xml->Attribute("relRotation");
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
	szTmp = xml->Attribute("relScale");
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

	szTmp = xml->Attribute("canVisit");
	if (szTmp)
		strTemp = szTmp;
	_canVisit = (strTemp == "false" ? false : true);

	return true;
}

Node::Node(const wstring& name, Node* parent, SceneManager* mgr)
	: _name(name)
	, _parent(parent)
	, _sceneMgr(mgr)
	, _canVisit(true)
{
	_relSpace._scale.set(1.0f, 1.0f, 1.0f);
	if (_parent)
		_parent->addChild(this);
	updateAbsPosition();
}

Node::~Node()
{
	remove();
	removeChildren();
}

void Node::visit(float dt)
{
	if (isCanVisit())
	{
		list<Node*>::iterator it = _children.begin();
		for (; it != _children.end(); ++it)
			(*it)->visit(dt);
	}
}

Matrix4x4 Node::getRelTransformation() const
{
	Matrix4x4 mat;
	mat.setRotationDegrees(_relSpace._rotation);
	mat.setTranslation(_relSpace._translation);
	if (_relSpace._scale != Vector3df(1,1,1))
	{
		Matrix4x4 smat;
		smat.setScale(_relSpace._scale);
		mat *= smat;
	}
	return mat;
}

bool Node::isCanVisit()
{
	return _canVisit;
}

void Node::setCanVisit(bool visible)
{
	_canVisit = visible;
}

const std::wstring& Node::getName() const
{
	return _name;
}

void Node::setName(wstring name)
{
	_name = name;
}

void Node::addChild(Node* child)
{
	if (child)
	{
		child->remove();
		_children.push_back(child);
		child->_parent = this;
	}
}

bool Node::removeChild(Node* child)
{
	list<Node*>::iterator it = _children.begin();
	for (; it != _children.end(); ++it)
	{
		if ((*it) == child)
		{
			child->_parent = NULL;
			_children.erase(it);
			return true;
		}
	}
	return false;
}

void Node::removeChildren()
{
	list<Node*>::iterator it = _children.begin();
	for (; it != _children.end(); ++it)
	{
		Node* child = (*it);
		child->_parent = NULL;
	}
	_children.clear();
}

void Node::remove()
{
	if (_parent)
		_parent->removeChild(this);
}

void Node::setParent(Node* newParent)
{
	remove();
	_parent = newParent;
	if (_parent)
		_parent->addChild(this);
}

Node* Node::getParent() const
{
	return _parent;
}

const Vector3df& Node::getScale() const
{
	return _relSpace._scale;
}

void Node::setScale(const Vector3df& scale)
{
	_relSpace._scale = scale;
}

const Vector3df& Node::getRotation() const
{
	return _relSpace._rotation;
}

void Node::setRotation(const Vector3df& rotation)
{
	_relSpace._rotation = rotation;
}

const Vector3df& Node::getPosition() const
{
	return _relSpace._translation;
}

void Node::setPosition(const Vector3df& newpos)
{
	_relSpace._translation = newpos;
}

Vector3df Node::getAbsPosition() const
{
	return _absTransformation.getTranslation();
}

void Node::animating(float dt)
{
	if (isCanVisit())
	{
		list<NodeAnimator*>::iterator ait = _animators.begin();
		while (ait != _animators.end())
		{
			NodeAnimator* anim = *ait;
			++ait;
			anim->animateNode(this, dt);
		}

		updateAbsPosition();
	}
}

void Node::addAnimator(NodeAnimator* animator)
{
	list<NodeAnimator*>::iterator it = std::find(_animators.begin(), _animators.end(), animator);
	if (it == _animators.end())
		_animators.push_back(animator);
}

bool Node::removeAnimator(NodeAnimator* animator)
{
	list<NodeAnimator*>::iterator it = std::find(_animators.begin(), _animators.end(), animator);
	if (it != _animators.end())
	{
		_animators.erase(it);
		return true;
	}

	return false;
}

void Node::removeAnimators()
{
	_animators.clear();
}

void Node::updateAbsPosition()
{
	if (_parent)
		_absTransformation = _parent->getAbsTransformation() * getRelTransformation();
	else
		_absTransformation = getRelTransformation();
}

NS_VAYO_END
