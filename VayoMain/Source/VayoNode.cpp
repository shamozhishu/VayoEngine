#include "VayoNode.h"
#include "VayoEntity.h"
#include "VayoMath.h"
#include "VayoNodeAnimator.h"

NS_VAYO_BEGIN

void Node::updateAbsPosition()
{
	if (_parent)
		_absTransformation = _parent->getAbsTransformation() * getRelTransformation();
	else
		_absTransformation = getRelTransformation();
}

Node::Node(const wstring& name, Node* parent, SceneManager* originSceneMgr)
	: _name(name)
	, _parent(parent)
	, _originSceneMgr(originSceneMgr)
	, _canVisit(true)
{
	if (_parent)
		_parent->addChild(this);
	updateAbsPosition();
}

Node::~Node()
{
	removeAnimators();
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
	if (!child || child->getOriginSceneMgr() != getOriginSceneMgr())
		return;
	child->remove();
	_children.push_back(child);
	child->_parent = this;
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
	if (!animator || animator->getOriginSceneMgr() != getOriginSceneMgr())
		return;
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

NS_VAYO_END
