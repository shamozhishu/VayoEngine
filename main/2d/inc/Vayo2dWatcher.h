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
#include "VayoMatrix3x3.h"
NS_VAYO2D_BEGIN

// 观察器基类
class _Vayo2dExport Watcher : public Body, public TouchDelegate, public KeypadDelegate
{
public:
	Watcher(const wstring& name, LayerManager* oriLayerMgr);
	virtual ~Watcher();
	virtual void  refresh();
	virtual void  setNeedRefresh(bool isRefresh);
	virtual void  moveTo(const Vector2df& pos);
	virtual void  moveBy(const Vector2df& pos);
	virtual void  scaleTo(const Vector2df& scale);
	virtual void  scaleBy(const Vector2df& scale);
	virtual void  rotateTo(float rot);
	virtual void  rotateBy(float rot);
	virtual const Matrix3x3& getView() const;
	virtual void  setWindow(Rectf window);
	virtual void  setViewport(Rectf viewport);
	virtual void  setViewAffector(const Matrix3x3& affector);
	virtual const Matrix3x3& getViewAffector() const;

private:
	PROPERTY_R_REF(Rectf, _window, Window)
	PROPERTY_R_REF(Rectf, _viewport, Viewport)
protected:
	Matrix3x3 _view;
	Matrix3x3 _affector;
	Matrix3x3 _orthoProj;
	float     _aspect;
	float     _rotation;
	bool      _needRefresh;
};

// 鸟瞰观察器
class _Vayo2dExport BirdEyeWatcher : public Watcher
{
	static Reflex<BirdEyeWatcher, const wstring&, LayerManager*> _dynReflex;
public:
	BirdEyeWatcher(const wstring& name, LayerManager* oriLayerMgr);
	~BirdEyeWatcher();
	void update(float dt);
	void setZoomSpeed(float speed);
	bool touchBegan(const Touch& touch, EMouseKeys key);
	void touchMoved(const Touch& touch, EMouseKeys key);
	void touchEnded(const Touch& touch, EMouseKeys key);
	bool touchWheel(const Touch& touch, float wheel);
	bool keyClicked(const tagKeyInput& keyInput);

private:
	float _zoomSpeed;
	bool  _enableMoved;
	bool  _zoomByMousePt;
};

NS_VAYO2D_END

#endif // __VAYO2D_WATCHER_H__
