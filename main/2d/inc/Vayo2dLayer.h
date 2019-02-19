/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* ��
\*************************************************************************/
#ifndef __VAYO2D_LAYER_H__
#define __VAYO2D_LAYER_H__

#include "Vayo2dSupport.h"
#include "Vayo2dJoint.h"
#include "math/VayoRectangle.h"
NS_VAYO2D_BEGIN

class Layer : public Joint
{
	static Reflex<Layer, const wstring&, Joint*, LayerManager*> _dynReflex;
public:
	Layer(const wstring& name, Joint* parent, LayerManager* oriLayerMgr);
	virtual ~Layer();
	virtual void         visit(float dt);
	virtual void         showWireBoundingArea(bool bShow);
	virtual bool         isShowWireBoundingArea() const;
	virtual const Rectf& getWorldArea() const;
	virtual void         attachBody(Body* body);
	int                  getAttachedBodiesCount() const;
	Body*                getAttacheBody(const wstring& name);
	virtual void         detachBody(Body* body);
	virtual Body*        detachBody(const wstring& name);
	virtual void         detachAllBodies();
	virtual Layer*       createChildLayer(const wstring& name = L"");

	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);
	const map<wstring, Body*>& getBodies() { return _bodies; }
	bool isAutomaticCulling() const { return _isAutomaticCulling; }
	void setAutomaticCulling(bool isCulling) { _isAutomaticCulling = isCulling; }

private:
	void updateWorldArea();
	void updateLocalArea();

private:
	map<wstring, Body*> _bodies;
	Rectf               _worldArea;
	Graphics*           _wireBoundingArea;
	bool                _showBoundingArea;
	bool                _isAutomaticCulling;
};

NS_VAYO2D_END

#endif // __VAYO2D_LAYER_H__
