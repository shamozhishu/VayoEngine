#include "Vayo2dJoint.h"

NS_VAYO2D_BEGIN

void Joint::updateAbsTransform()
{
	if (_parent)
		_absTransform = _parent->getAbsTransform() * getRelTransform();
	else
		_absTransform = getRelTransform();
}

Joint::Joint(const wstring& name, Joint* parent, LayerManager* oriLayerMgr)
	: _name(name)
	, _canVisit(true)
	, _position(0, 0)
	, _scale(1, 1)
	, _rotAngle(0)
	, _parent(parent)
	, _oriLayerMgr(oriLayerMgr)
{
	if (_parent)
		_parent->addChild(this);
	updateAbsTransform();
}

Joint::~Joint()
{
	remove();
	removeChildren();
}

void Joint::visit(float dt)
{
	if (isCanVisit())
	{
		list<Joint*>::iterator it = _children.begin();
		for (; it != _children.end(); ++it)
			(*it)->visit(dt);
	}
}

Matrix3x3 Joint::getRelTransform() const
{
	Matrix3x3 mat;
	mat.setRotationDegrees(_rotAngle);
	mat.setTranslation(_position);
	if (_scale != Vector2df(1, 1))
	{
		Matrix3x3 smat;
		smat.setScale(_scale);
		mat *= smat;
	}
	return mat;
}

bool Joint::isCanVisit()
{
	return _canVisit;
}

void Joint::setCanVisit(bool visit)
{
	_canVisit = visit;
}

const wstring& Joint::getName() const
{
	return _name;
}

void Joint::addChild(Joint* child)
{
	if (!child || child->getOriLayerMgr() != getOriLayerMgr())
		return;
	child->remove();
	_children.push_back(child);
	child->_parent = this;
}

bool Joint::removeChild(Joint* child)
{
	list<Joint*>::iterator it = _children.begin();
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

void Joint::removeChildren()
{
	list<Joint*>::iterator it = _children.begin();
	for (; it != _children.end(); ++it)
	{
		Joint* child = (*it);
		child->_parent = NULL;
	}
	_children.clear();
}

void Joint::remove()
{
	if (_parent)
		_parent->removeChild(this);
}

void Joint::setParent(Joint* newParent)
{
	remove();
	_parent = newParent;
	if (_parent)
		_parent->addChild(this);
}

Joint* Joint::getParent() const
{
	return _parent;
}

const Vector2df& Joint::getScale() const
{
	return _scale;
}

void Joint::setScale(const Vector2df& scale)
{
	_scale = scale;
}

float Joint::getRotAngle() const
{
	return _rotAngle;
}

void Joint::setRotAngle(float rotAngle)
{
	_rotAngle = rotAngle;
}

const Vector2df& Joint::getPosition() const
{
	return _position;
}

void Joint::setPosition(const Vector2df& newpos)
{
	_position = newpos;
}

Vector2df Joint::getAbsPosition() const
{
	return _absTransform.getTranslation();
}

NS_VAYO2D_END
