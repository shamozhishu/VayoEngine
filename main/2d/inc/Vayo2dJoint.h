/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 关节
\*************************************************************************/
#ifndef __VAYO2D_JOINT_H__
#define __VAYO2D_JOINT_H__

#include "Vayo2dSupport.h"
#include "VayoUserDataBind.h"
#include "math/VayoMatrix3x3.h"
#include "VayoAttribSerializer.h"
NS_VAYO2D_BEGIN

class Joint : public AttribSerializer
{
public:
	Joint(const wstring& name, Joint* parent, LayerManager* oriLayerMgr);
	virtual ~Joint();
	virtual void             visit(float dt);
	virtual Matrix3x3        getRelTransform() const;
	virtual bool             isCanVisit();
	virtual void             setCanVisit(bool visit);
	virtual const wstring&   getName() const;
	virtual void             addChild(Joint* child);
	virtual bool             removeChild(Joint* child);
	virtual void             removeChildren();
	virtual void             remove();
	virtual void             setParent(Joint* newParent);
	virtual Joint*           getParent() const;
	virtual const Vector2df& getScale() const;
	virtual void             setScale(const Vector2df& scale);
	virtual float            getRotAngle() const;
	virtual void             setRotAngle(float rotAngle);
	virtual const Vector2df& getPosition() const;
	virtual void             setPosition(const Vector2df& newpos);
	virtual Vector2df        getAbsPosition() const;

	const list<Joint*>& getChildren() const { return _children; }
	const Matrix3x3&    getAbsTransform() const { return _absTransform; }
	const UserDataBind& getUserDataBind() const { return _userDataBind; }
	UserDataBind&       getUserDataBind() { return _userDataBind; }

protected:
	virtual void updateAbsTransform();
	DISALLOW_COPY_AND_ASSIGN(Joint)
	PROPERTY_R(LayerManager*, _oriLayerMgr, OriLayerMgr)
protected:
	wstring      _name;
	bool         _canVisit;
	Vector2df    _position;
	Vector2df    _scale;
	float        _rotAngle;
	Joint*       _parent;
	list<Joint*> _children;
	Matrix3x3    _absTransform;
	UserDataBind _userDataBind;
};

NS_VAYO2D_END

#endif // __VAYO2D_JOINT_H__
