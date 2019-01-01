/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 节点动画
\*************************************************************************/
#ifndef __VAYO_NODE_ANIMATOR_H__
#define __VAYO_NODE_ANIMATOR_H__

#include "VayoInput.h"
#include "VayoAttribSerializer.h"
NS_VAYO_BEGIN

class _VayoExport NodeAnimator : public AttribSerializer, public TouchDelegate, public KeypadDelegate
{
public:
	NodeAnimator(const wstring& name, SceneManager* originSceneMgr);
	virtual ~NodeAnimator();
	virtual void animateNode(Node* node, float dt) = 0;
	virtual bool hasFinished() const;
	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);

protected:
	PROPERTY_R_REF(wstring, _name, Name)
	PROPERTY_R(SceneManager*, _originSceneMgr, OriginSceneMgr)
};

NS_VAYO_END

#endif // __VAYO_NODE_ANIMATOR_H__
