/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 动作
\*************************************************************************/
#ifndef __VAYO2D_ACTION_H__
#define __VAYO2D_ACTION_H__

#include "Vayo2dSupport.h"
#include "VayoInput.h"
#include "VayoAttribSerializer.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Action : public AttribSerializer, public TouchDelegate, public KeypadDelegate
{
public:
	Action(const wstring& name, LayerManager* oriLayerMgr);
	virtual ~Action();
	virtual void animateJoint(Joint* joint, float dt) = 0;
	virtual bool hasFinished() const;
	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);

protected:
	PROPERTY_R_REF(wstring, _name, Name)
	PROPERTY_R(LayerManager*, _oriLayerMgr, OriLayerMgr)
};

NS_VAYO2D_END

#endif // __VAYO2D_ACTION_H__
