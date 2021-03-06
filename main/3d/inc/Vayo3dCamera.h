/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 摄像机
\*************************************************************************/
#ifndef __VAYO3D_CAMERA_H__
#define __VAYO3D_CAMERA_H__

#include "VayoInput.h"
#include "VayoBitState.h"
#include "VayoRectangle.h"
#include "Vayo3dArcball.h"
#include "Vayo3dFrustum.h"
#include "Vayo3dViewMemento.h"
#include "Vayo3dMovableObject.h"

NS_VAYO3D_BEGIN

// 摄像机基类
class _Vayo3dExport Camera : public MovableObject, public TouchDelegate, public KeypadDelegate
{
	friend class ViewMemento;
public:
	Camera(const wstring& name, SceneManager* oriSceneMgr);
	virtual ~Camera();
	virtual void  refresh();
	virtual void  setNeedRefresh(bool isRefresh);
	virtual bool  isOrthogonal() { return false; }
	virtual bool  getWorldPos(Vector3df& outWorldPos) const;
	virtual bool  getWorldLook(Vector3df& outWorldLook) const;

	virtual const Vector3df& getRight() const;
	virtual const Vector3df& getUp() const;
	virtual const Vector3df& getLook() const;

	virtual void setLens(float fovY, float aspect, float zn, float zf);
	virtual void lookAt(const Vector3df& pos, const Vector3df& target = Vector3df::Origin, const Vector3df& worldUp = Vector3df::YAxis);

	virtual const Matrix4x4& getView() const;
	virtual const Matrix4x4& getProj() const;

	virtual void             setViewAffector(const Matrix4x4& affector);
	virtual const Matrix4x4& getViewAffector() const;

	virtual void strafe(float d);
	virtual void fly(float d);
	virtual void walk(float d);

	virtual void pitch(float angle);
	virtual void yaw(float angle);
	virtual void roll(float angle);

	virtual void rotateX(float angle);
	virtual void rotateY(float angle);
	virtual void rotateZ(float angle);

	virtual void transform(const Matrix4x4& mat, bool bRotationOnly=false);

	virtual float getNearZ() const;
	virtual float getFarZ() const;
	virtual float getFovY() const;
	virtual float getFovX() const;
	virtual float getAspect() const;

	virtual float getNearWindowWidth() const;
	virtual float getNearWindowHeight() const;
	virtual float getFarWindowWidth() const;
	virtual float getFarWindowHeight() const;

	virtual const Frustum& getViewFrustum() const;

	virtual bool setViewMemento(const wstring& name);
	virtual bool getViewMemento(const wstring& name);
	virtual bool hasViewMemento(const wstring& name);

	virtual void serialize(XMLElement* outXml);
	virtual bool deserialize(XMLElement* inXml);

	/* 获取屏幕坐标对应的视图空间内的位置 */
	Vector3df getPosInViewSpace(int xScreen, int yScreen);
	/* 获取视图空间内的拾取射线方向(拾取射线的起点为(0,0,0)) */
	Vector3df getRayInViewSpace(int xScreen, int yScreen);

protected:
	virtual void regenerateViewArea();
	virtual void recalculateViewArea();
	virtual ViewMementoPtr createViewMemento() = 0;
	map<wstring, ViewMementoPtr> _viewMementoPool;

protected:
	bool      _needRefresh;
	Vector3df _position;
	Vector3df _right;
	Vector3df _up;
	Vector3df _look;
	Matrix4x4 _view;
	Matrix4x4 _affector;
	float     _nearZ;
	float     _farZ;
	float     _fovY;
	float     _aspect;
	float     _nearWindowHeight;
	float     _farWindowHeight;
	Frustum   _viewArea;
};

// 第一人称摄像机
class _Vayo3dExport FPSCamera : public Camera
{
	friend class ViewMementoFPS;
	static Reflex<FPSCamera, const wstring&, SceneManager*> _dynReflex;
public:
	FPSCamera(const wstring& name, SceneManager* oriSceneMgr);
	~FPSCamera();
	void update(float dt);
	void setMoveSpeed(float speed);
	void touchMoved(const Touch& touch, EMouseKeys key);
	bool keyClicked(const tagKeyInput& keyInput);
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);

protected:
	ViewMementoPtr createViewMemento();

protected:
	float    _moveSpeed[2];
	BitState _direction;
};

// 轨道摄像机
class _Vayo3dExport OrbitCamera : public Camera
{
	friend class ViewMementoOrbit;
	static Reflex<OrbitCamera, const wstring&, SceneManager*> _dynReflex;
public:
	OrbitCamera(const wstring& name, SceneManager* oriSceneMgr);
	~OrbitCamera();
	void update(float dt);
	void resetArcball();
	void setMoveSpeed(float speed);
	void setZoomSpeed(float speed);
	void setZoomFactor(float zoom);
	void setLens(float fovY, float aspect, float zn, float zf);
	void lookAt(const Vector3df& pos, const Vector3df& target = Vector3df::Origin, const Vector3df& worldUp = Vector3df::YAxis);
	bool touchBegan(const Touch& touch, EMouseKeys key);
	void touchMoved(const Touch& touch, EMouseKeys key);
	void touchEnded(const Touch& touch, EMouseKeys key);
	bool touchWheel(const Touch& touch, float wheel);
	bool keyClicked(const tagKeyInput& keyInput);
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);

protected:
	void regenerateViewArea();
	ViewMementoPtr createViewMemento();

protected:
	float   _moveSpeed[2];
	float   _zoomSpeed[2];
	float   _zoomFactor;
	Arcball _arcball;
};

// 鹰眼摄像机
class _Vayo3dExport EagleEyeCamera : public OrbitCamera
{
	friend class ViewMementoEagleEye;
	static Reflex<EagleEyeCamera, const wstring&, SceneManager*> _dynReflex;
public:
	EagleEyeCamera(const wstring& name, SceneManager* oriSceneMgr);
	~EagleEyeCamera();
	void  update(float dt);
	bool  isOrthogonal() { return true; }
	void  forbidRotaion(bool isForbid);
	void  setLens(float widthOfViewVolume, float heightOfViewVolume, float zn, float zf);
	void  setLens(float left, float right, float bottom, float top, float zn, float zf);
	float getFovY() const;
	float getFovX() const;
	float getNearWindowWidth() const;
	float getNearWindowHeight() const;
	float getFarWindowWidth() const;
	bool  touchBegan(const Touch& touch, EMouseKeys key);
	void  touchMoved(const Touch& touch, EMouseKeys key);
	void  touchEnded(const Touch& touch, EMouseKeys key);
	bool  touchWheel(const Touch& touch, float wheel);
	bool  keyClicked(const tagKeyInput& keyInput);

protected:
	void regenerateViewArea();
	ViewMementoPtr createViewMemento();
private:
	bool      _canRot;
	bool      _cursorZoom;
	Rectf     _eyeArea;
	Matrix4x4 _offsetMat;
};

NS_VAYO3D_END

#endif // __VAYO3D_CAMERA_H__
