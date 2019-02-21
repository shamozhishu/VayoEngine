/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 观察器
\*************************************************************************/
#ifndef __VAYO2D_WATCHER_H__
#define __VAYO2D_WATCHER_H__

#include "Vayo2dSupport.h"
#include "Vayo2dBody.h"
#include "VayoInput.h"
#include "math/VayoMatrix3x3.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Watcher : public Body, public TouchDelegate, public KeypadDelegate
{
	static Reflex<Watcher, const wstring&, LayerManager*> _dynReflex;
public:
	Watcher(const wstring& name, LayerManager* oriLayerMgr);
	virtual ~Watcher();
	virtual void  refresh();
	virtual void  update(float dt);
	virtual void  moveTo(const Vector2df& pos);
	virtual void  moveBy(const Vector2df& pos);
	virtual void  scaleTo(const Vector2df& scale);
	virtual void  scaleBy(const Vector2df& scale);
	virtual void  rotateTo(float rot);
	virtual void  rotateBy(float rot);
	virtual const Matrix3x3& getView() const;
	virtual void  setViewAffector(const Matrix3x3& affector);
	virtual const Matrix3x3& getViewAffector() const;
	virtual bool  touchBegan(const Touch& touch, EMouseKeys key);
	virtual void  touchMoved(const Touch& touch, EMouseKeys key);
	virtual void  touchEnded(const Touch& touch, EMouseKeys key);
	virtual bool  touchWheel(const Touch& touch, float wheel);
	virtual bool  keyClicked(const tagKeyInput& keyInput);

protected:
	Matrix3x3 _view;
	Matrix3x3 _affector;
	float     _rotation;
	float     _zoomFactor;
	bool      _enableMoved;
};

NS_VAYO2D_END

#endif // __VAYO2D_WATCHER_H__
