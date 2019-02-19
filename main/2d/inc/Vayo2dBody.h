/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 对象基类
\*************************************************************************/
#ifndef __VAYO2D_BODY_H__
#define __VAYO2D_BODY_H__

#include "Vayo2dSupport.h"
#include "VayoBitState.h"
#include "VayoUserDataBind.h"
#include "math/VayoRectangle.h"
#include "VayoAttribSerializer.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Body : public AttribSerializer
{
	friend class Layer;
public:
	Body(const wstring& name, LayerManager* oriLayerMgr);
	virtual ~Body();
	virtual void           update(float dt) = 0;
	virtual const wstring& getName() const;
	virtual Layer*         getParentLayer() const;
	virtual bool           isVisible() const;
	virtual void           setVisible(bool visible);
	virtual Rectf          getWorldArea() const;
	virtual const Rectf&   getLocalArea() const;
	virtual void           refreshLocalArea();
	BitState&              getCollideMask();
	UserDataBind&          getUserDataBind();
	const UserDataBind&    getUserDataBind() const;
	void                   serialize(XMLElement* outXml);
	bool                   deserialize(XMLElement* inXml);

protected:
	DISALLOW_COPY_AND_ASSIGN(Body)
	PROPERTY_RW(unsigned int, _queueID, QueueID)
	PROPERTY_R(LayerManager*, _oriLayerMgr, OriLayerMgr)
protected:
	wstring      _name;
	Layer*       _parentLayer;
	bool         _visible;
	Rectf        _localArea;
	BitState     _collideMask;
	UserDataBind _userDataBind;
};

NS_VAYO2D_END

#endif // __VAYO2D_BODY_H__
