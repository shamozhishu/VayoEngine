#include "Vayo3dCamera.h"
#include "Vayo3dRoot.h"
#include "Vayo3dDevice.h"
#include "Vayo3dRenderSystem.h"
#include "Vayo3dSceneNode.h"
#include "Vayo3dSceneManager.h"
#include "VayoUtils.h"

NS_VAYO3D_BEGIN

Vector3df getRayInViewSpace(int xScreen, int yScreen)
{
	Camera* pCamera = Root::getSingleton().getCurSceneMgr()->getActiveCamera();
	if (!pCamera)
		return Vector3df::Origin;

	Matrix4x4 projMat = pCamera->getProj();
	if (!projMat.makeInverse())
		return Vector3df::Origin;

	Recti viewPort = Root::getSingleton().getActiveRenderer()->getViewPort();
	float xN = (float)(xScreen - viewPort._upperLeftCorner._x) * 2.0f / (float)viewPort.getWidth() - 1.0f;
	float yN = (float)(-yScreen + viewPort._upperLeftCorner._y) * 2.0f / (float)viewPort.getHeight() + 1.0f;
	float wC = pCamera->getNearZ();
	bool isOrtho = pCamera->isOrthogonal();
	float xC = isOrtho ? xN : xN * wC;
	float yC = isOrtho ? yN : yN * wC;
	Vector3df rayDir(xC, yC, 0.0f);
	projMat.transformVect(rayDir);
	rayDir.normalize();
	return rayDir;
}

//////////////////////////////////////////////////////////////////////////
void Camera::setNeedUpdate(bool isUpdate)
{
	_needUpdate = isUpdate;
}

Camera::Camera(const wstring& name, SceneManager* originSceneMgr)
	: MovableObject(name, originSceneMgr)
	, TouchDelegate(originSceneMgr->getName())
	, KeypadDelegate(originSceneMgr->getName())
	, _right(1.0f, 0.0f, 0.0f)
	, _up(0.0f, 1.0f, 0.0f)
	, _look(0.0f, 0.0f, -1.0f)
	, _needUpdate(true)
{
	const Dimension2di& size = Root::getSingleton().getActiveDevice()->getScreenSize();
	_aspect = (float)size._width / size._height;
	_nearZ = 1.0f;
	_farZ = 1000.0f;
}

Camera::~Camera()
{
}

void Camera::refresh()
{
	if (_needUpdate)
	{
		updateViewTransform();
		_needUpdate = false;
	}

	regenerateViewArea();
	recalculateViewArea();
	Root::getSingleton().getActiveRenderer()->setTransform(ETS_VIEW, getView());
	Root::getSingleton().getActiveRenderer()->setTransform(ETS_PROJECTION, getProj());
}

bool Camera::getWorldPos(Vector3df& outWorldPos) const
{
	Matrix4x4 worldTransform = _viewArea.getTransform(Frustum::EFT_VIEW);
	if (!worldTransform.makeInverse())
		return false;
	outWorldPos = worldTransform.getTranslation();
	return true;
}

bool Camera::getWorldLook(Vector3df& outWorldLook) const
{
	Matrix4x4 worldTransform = _viewArea.getTransform(Frustum::EFT_VIEW);
	if (!worldTransform.makeInverse())
		return false;
	outWorldLook.set(-worldTransform[8], -worldTransform[9], -worldTransform[10]);
	outWorldLook.normalize();
	return true;
}

const Vector3df& Camera::getRight() const
{
	return _right;
}

const Vector3df& Camera::getUp() const
{
	return _up;
}

const Vector3df& Camera::getLook() const
{
	return _look;
}

void Camera::setLens(float fovY, float aspect, float zn, float zf)
{
	_fovY = m3dDegToRad(fovY);
	_aspect = aspect;
	_nearZ = zn;
	_farZ = zf;
	_nearWindowHeight = 2.0f * _nearZ * tanf(0.5f*_fovY);
	_farWindowHeight = 2.0f * _farZ * tanf(0.5f*_fovY);
	_viewArea.getTransform(Frustum::EFT_PROJECTION).buildProjectionMatrixPerspectiveFovRH(_fovY, _aspect, _nearZ, _farZ);
}

void Camera::lookAt(const Vector3df& pos, const Vector3df& target /*= Vector3df::Origin*/, const Vector3df& worldUp /*= Vector3df::YAxis*/)
{
	_position = pos;
	Vector3df zaxis = pos - target;
	zaxis.normalize();
	Vector3df xaxis = worldUp.crossProduct(zaxis);
	xaxis.normalize();
	Vector3df yaxis = zaxis.crossProduct(xaxis);
	_right = xaxis;
	_up = yaxis;
	_look = zaxis;
	_look.invert();
	_view.buildCameraLookAtMatrixRH(pos, target, worldUp);
}

const Matrix4x4& Camera::getView() const
{
	return _viewArea.getTransform(Frustum::EFT_VIEW);
}

const Matrix4x4& Camera::getProj() const
{
	return _viewArea.getTransform(Frustum::EFT_PROJECTION);
}

void Camera::setViewAffector(const Matrix4x4& affector)
{
	_affector = affector;
}

const Matrix4x4& Camera::getViewAffector() const
{
	return _affector;
}

void Camera::strafe(float d)
{
	_position += _right * d;
	_needUpdate = true;
}

void Camera::fly(float d)
{
	_position += _up * d;
	_needUpdate = true;
}

void Camera::walk(float d)
{
	_position += _look * d;
	_needUpdate = true;
}

void Camera::pitch(float angle)
{
	Matrix4x4 rotMat;
	rotMat.setRotationDegrees(angle, _right);
	rotMat.transformVect(_up);
	rotMat.transformVect(_look);
	_needUpdate = true;
}

void Camera::yaw(float angle)
{
	Matrix4x4 rotMat;
	rotMat.setRotationDegrees(angle, _up);
	rotMat.transformVect(_look);
	rotMat.transformVect(_right);
	_needUpdate = true;
}

void Camera::roll(float angle)
{
	Matrix4x4 rotMat;
	rotMat.setRotationDegrees(angle, _look*(-1));
	rotMat.transformVect(_right);
	rotMat.transformVect(_up);
	_needUpdate = true;
}

void Camera::rotateX(float angle)
{
	Matrix4x4 rotMat;
	rotMat.setRotationDegrees(angle, Vector3df::XAxis);
	transform(rotMat, true);
}

void Camera::rotateY(float angle)
{
	Matrix4x4 rotMat;
	rotMat.setRotationDegrees(angle, Vector3df::YAxis);
	transform(rotMat, true);
}

void Camera::rotateZ(float angle)
{
	Matrix4x4 rotMat;
	rotMat.setRotationDegrees(angle, Vector3df::ZAxis);
	transform(rotMat, true);
}

void Camera::transform(const Matrix4x4& mat, bool bRotationOnly/*=false*/)
{
	mat.transformVect(_right);
	mat.transformVect(_up);
	mat.transformVect(_look);
	if (!bRotationOnly)
		mat.translateVect(_position);
	_needUpdate = true;
}

float Camera::getNearZ() const
{
	return _nearZ;
}

float Camera::getFarZ() const
{
	return _farZ;
}

float Camera::getFovY() const
{
	return m3dRadToDeg(_fovY);
}

float Camera::getFovX() const
{
	float halfWidth = 0.5f*getNearWindowWidth();
	return m3dRadToDeg(2.0f*atan(halfWidth / _nearZ));
}

float Camera::getAspect() const
{
	return _aspect;
}

float Camera::getNearWindowWidth() const
{
	return _aspect * _nearWindowHeight;
}

float Camera::getNearWindowHeight() const
{
	return _nearWindowHeight;
}

float Camera::getFarWindowWidth() const
{
	return _aspect * _farWindowHeight;
}

float Camera::getFarWindowHeight() const
{
	return _farWindowHeight;
}

const Frustum& Camera::getViewFrustum() const
{
	return _viewArea;
}

void Camera::updateViewTransform()
{
	Vector3df yaxis = _up;
	Vector3df zaxis = _look;
	zaxis.invert();
	zaxis.normalize();
	Vector3df xaxis = yaxis.crossProduct(zaxis);
	xaxis.normalize();
	yaxis = zaxis.crossProduct(xaxis);

	_view(0, 0) = xaxis._x;
	_view(1, 0) = yaxis._x;
	_view(2, 0) = zaxis._x;
	_view(3, 0) = 0;

	_view(0, 1) = xaxis._y;
	_view(1, 1) = yaxis._y;
	_view(2, 1) = zaxis._y;
	_view(3, 1) = 0;

	_view(0, 2) = xaxis._z;
	_view(1, 2) = yaxis._z;
	_view(2, 2) = zaxis._z;
	_view(3, 2) = 0;

	_view(0, 3) = -xaxis.dotProduct(_position);
	_view(1, 3) = -yaxis.dotProduct(_position);
	_view(2, 3) = -zaxis.dotProduct(_position);
	_view(3, 3) = 1.0f;
}

bool Camera::setViewMemento(const wstring& name)
{
	if (name == L"")
		return false;
	ViewMementoPtr viewPtr = createViewMemento();
	viewPtr->storage(this);
	_viewMementoPool[name] = viewPtr;
	return true;
}

bool Camera::getViewMemento(const wstring& name)
{
	map<wstring, ViewMementoPtr>::iterator it = _viewMementoPool.find(name);
	if (it == _viewMementoPool.end())
		return false;

	ViewMementoPtr viewPtr = it->second;
	viewPtr->recover(this);
	_viewMementoPool.erase(it);
	_needUpdate = true;
	return true;
}

bool Camera::hasViewMemento(const wstring& name)
{
	map<wstring, ViewMementoPtr>::iterator it = _viewMementoPool.find(name);
	return it != _viewMementoPool.end();
}

void Camera::regenerateViewArea()
{
	_viewArea.getTransform(Frustum::EFT_VIEW) = _view * _affector;
}

void Camera::recalculateViewArea()
{
	getWorldPos(_viewArea._cameraPosition);
	Matrix4x4 m = _viewArea.getTransform(Frustum::EFT_PROJECTION) * _viewArea.getTransform(Frustum::EFT_VIEW);
	_viewArea.setFrom(m);
}

void Camera::serialize(XMLElement* outXml)
{
	MovableObject::serialize(outXml);
	char szbuf[256];
	sprintf_s(szbuf, sizeof(szbuf), "%f,%f,%f", _position._x, _position._y, _position._z);
	outXml->SetAttribute("position", szbuf);
	Vector3df targetPos = _position + (_look*10.0f);
	sprintf_s(szbuf, sizeof(szbuf), "%f,%f,%f", targetPos._x, targetPos._y, targetPos._z);
	outXml->SetAttribute("target", szbuf);
	sprintf_s(szbuf, sizeof(szbuf), "%f,%f,%f", _up._x, _up._y, _up._z);
	outXml->SetAttribute("worldUp", szbuf);
	if (!isOrthogonal())
		outXml->SetAttribute("fovY", getFovY());
	outXml->SetAttribute("zn", getNearZ());
	outXml->SetAttribute("zf", getFarZ());
}

bool Camera::deserialize(XMLElement* inXml)
{
	if (!MovableObject::deserialize(inXml))
		return false;

	Vector3df position(0, 0, 0), target(0, 0, -1), worldUp(0, 1, 0);
	vector<string> container;
	string strTemp = inXml->Attribute("position");
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		position.set(x, y, z);
	}

	strTemp = inXml->Attribute("target");
	container.clear();
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		target.set(x, y, z);
	}

	strTemp = inXml->Attribute("worldUp");
	container.clear();
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		worldUp.set(x, y, z);
	}

	lookAt(position, target, worldUp);

	float zn = inXml->FloatAttribute("zn");
	float zf = inXml->FloatAttribute("zf");

	if (isOrthogonal())
		setLens(_nearWindowHeight, _farWindowHeight, zn, zf);
	else
		setLens(inXml->FloatAttribute("fovY"), getAspect(), zn, zf);

	return true;
}

//////////////////////////////////////////////////////////////////////////
Reflex<FPSCamera, const wstring&, SceneManager*> FPSCamera::_dynReflex;
FPSCamera::FPSCamera(const wstring& name, SceneManager* originSceneMgr)
	: Camera(name, originSceneMgr)
{
	_moveSpeed[0] = _moveSpeed[1] = 20.0f;
}

FPSCamera::~FPSCamera()
{
}

void FPSCamera::update(float dt)
{
	if (Root::getSingleton().getCurSceneMgr()->getActiveCamera() != this)
		return;
	if (_direction.checkState(EDIRECTION_UPWARD))
		fly(_moveSpeed[0] * dt);
	if (_direction.checkState(EDIRECTION_DOWNWARD))
		fly(-_moveSpeed[0] * dt);
	if (_direction.checkState(EDIRECTION_LEFTWARD))
		strafe(-_moveSpeed[0] * dt);
	if (_direction.checkState(EDIRECTION_RIGHTWARD))
		strafe(_moveSpeed[0] * dt);
	if (_direction.checkState(EDIRECTION_FORWARD))
		walk(_moveSpeed[0] * dt);
	if (_direction.checkState(EDIRECTION_BACKWARD))
		walk(-_moveSpeed[0] * dt);
	Camera::refresh();
}

void FPSCamera::setMoveSpeed(float speed)
{
	_moveSpeed[0] = _moveSpeed[1] = speed;
}

void FPSCamera::touchMoved(const Touch& touch, EMouseKeys key)
{
	if (key == EMK_LEFT)
	{
		float dx = 0.1f * touch.getDelta()._x;
		float dy = 0.1f * touch.getDelta()._y;
		pitch(-dy);
		rotateY(-dx);
	}
}

bool FPSCamera::keyClicked(const tagKeyInput& keyInput)
{
	bool ret = false;
	unsigned int uCurDire = 0;
	switch (keyInput.Key)
	{
	case KEY_KEY_W: uCurDire = EDIRECTION_FORWARD; ret = true; break;
	case KEY_KEY_S: uCurDire = EDIRECTION_BACKWARD; ret = true; break;
	case KEY_KEY_A: uCurDire = EDIRECTION_LEFTWARD; ret = true; break;
	case KEY_KEY_D: uCurDire = EDIRECTION_RIGHTWARD; ret = true; break;
	case KEY_KEY_R: uCurDire = EDIRECTION_UPWARD; ret = true; break;
	case KEY_KEY_F: uCurDire = EDIRECTION_DOWNWARD; ret = true; break;
	default: _direction.clearState();break;
	}

	if (keyInput.PressedDown)
		_direction.addState(uCurDire);
	else
		_direction.eraseState(uCurDire);

	if (keyInput.Control)
		_moveSpeed[0] = _moveSpeed[1] * 2;
	else
		_moveSpeed[0] = _moveSpeed[1];

	return ret;
}

void FPSCamera::regenerateViewArea()
{
	SceneNode* pParent = getParentNode();
	if (pParent)
		_viewArea.getTransform(Frustum::EFT_VIEW) = _view * pParent->getAbsTransformation() * _affector;
	else
		Camera::regenerateViewArea();
}

ViewMementoPtr FPSCamera::createViewMemento()
{
	return ViewMementoPtr(new ViewMementoFPS());
}

void FPSCamera::serialize(XMLElement* outXml)
{
	Camera::serialize(outXml);
	outXml->SetAttribute("moveSpeed", _moveSpeed[1]);
}

bool FPSCamera::deserialize(XMLElement* inXml)
{
	if (!Camera::deserialize(inXml))
		return false;
	_moveSpeed[0] = _moveSpeed[1] = inXml->FloatAttribute("moveSpeed");
	return true;
}

//////////////////////////////////////////////////////////////////////////
Reflex<OrbitCamera, const wstring&, SceneManager*> OrbitCamera::_dynReflex;
OrbitCamera::OrbitCamera(const wstring& name, SceneManager* originSceneMgr)
	: Camera(name, originSceneMgr)
	, _arcball(0, 0)
{
	_moveSpeed[0] = _moveSpeed[1] = _zoomSpeed[0] = _zoomSpeed[1] = 5.0f;
	const Dimension2di& size = Root::getSingleton().getActiveDevice()->getScreenSize();
	_arcball.setBounds(size._width, size._height);
	_position.set(0, 0, 100);
}

OrbitCamera::~OrbitCamera()
{
}

void OrbitCamera::update(float dt)
{
	if (Root::getSingleton().getCurSceneMgr()->getActiveCamera() != this)
		return;
	Camera::refresh();
}

void OrbitCamera::resetArcball()
{
	_arcball._transform.makeIdentity();
	_arcball._thisRot.makeIdentity();
	_arcball._lastRot.makeIdentity();
	_arcball._deltaRot.makeIdentity();
}

void OrbitCamera::setMoveSpeed(float speed)
{
	_moveSpeed[0] = _moveSpeed[1] = speed;
}

void OrbitCamera::setZoomSpeed(float speed)
{
	_zoomSpeed[0] = _zoomSpeed[1] = speed;
}

void OrbitCamera::setLens(float fovY, float aspect, float zn, float zf)
{
	Camera::setLens(fovY, aspect, zn, zf);
	Dimension2di size = Root::getSingleton().getActiveDevice()->getScreenSize();
	_arcball.setBounds(size._width, size._height);
}

void OrbitCamera::lookAt(const Vector3df& pos, const Vector3df& target /*= Vector3df::Origin*/, const Vector3df& worldUp /*= Vector3df::YAxis*/)
{
	float zpos = max_(max_(pos._x, pos._y), pos._z);
	_position.set(0, 0, zpos);
	_affector.buildCameraLookAtMatrixRH(pos, target, worldUp);
	_affector.setTranslation(Vector3df::Origin);
	_needUpdate = true;
}

bool OrbitCamera::touchBegan(const Touch& touch, EMouseKeys key)
{
	switch (key)
	{
	case EMK_LEFT:
		_arcball.updateRotate(touch.getCurPos()._x, touch.getCurPos()._y, true);
		return true;
	}
	return false;
}

void OrbitCamera::touchMoved(const Touch& touch, EMouseKeys key)
{
	switch (key)
	{
	case EMK_LEFT:
		_arcball.updateRotate(touch.getCurPos()._x, touch.getCurPos()._y, true);
		break;
	case EMK_RIGHT:
		_arcball.updateTranslate(touch.getDelta()._x * _moveSpeed[0], -touch.getDelta()._y * _moveSpeed[0], 0);
		break;
	}
}

void OrbitCamera::touchEnded(const Touch& touch, EMouseKeys key)
{
	switch (key)
	{
	case EMK_LEFT:
		_arcball.updateRotate(touch.getCurPos()._x, touch.getCurPos()._y, false);
		break;
	}
}

bool OrbitCamera::touchWheel(const Touch& touch, float wheel)
{
	Matrix4x4 transformMat, invMat;
	invMat = _view;
	if (invMat.makeInverse())
		transformMat = invMat;

	Vector3df rayDir = getRayInViewSpace(touch.getCurPos()._x, touch.getCurPos()._y);
	transformMat.rotateVect(rayDir);
	rayDir.normalize();
	float factor = wheel*_zoomSpeed[0];
	rayDir *= factor;
	_position -= rayDir;
	_needUpdate = true;

	return true;
}

bool OrbitCamera::keyClicked(const tagKeyInput& keyInput)
{
	if (keyInput.Control && keyInput.PressedDown)
	{
		_moveSpeed[0] = _moveSpeed[1] * 2;
		_zoomSpeed[0] = _zoomSpeed[1] * 2;
	}
	else
	{
		_moveSpeed[0] = _moveSpeed[1];
		_zoomSpeed[0] = _zoomSpeed[1];
	}
	return false;
}

void OrbitCamera::regenerateViewArea()
{
	SceneNode* pParent = getParentNode();
	if (pParent)
		_viewArea.getTransform(Frustum::EFT_VIEW) = _view * pParent->getAbsTransformation() * _arcball._transform * _affector;
	else
		_viewArea.getTransform(Frustum::EFT_VIEW) = _view * _arcball._transform * _affector;
}

ViewMementoPtr OrbitCamera::createViewMemento()
{
	return ViewMementoPtr(new ViewMementoOrbit());
}

void OrbitCamera::serialize(XMLElement* outXml)
{
	Camera::serialize(outXml);
	outXml->SetAttribute("moveSpeed", _moveSpeed[1]);
	outXml->SetAttribute("zoomSpeed", _zoomSpeed[1]);
}

bool OrbitCamera::deserialize(XMLElement* inXml)
{
	if (!Camera::deserialize(inXml))
		return false;
	_moveSpeed[0] = _moveSpeed[1] = inXml->FloatAttribute("moveSpeed");
	_zoomSpeed[0] = _zoomSpeed[1] = inXml->FloatAttribute("zoomSpeed");
	return true;
}

//////////////////////////////////////////////////////////////////////////
Reflex<EagleEyeCamera, const wstring&, SceneManager*> EagleEyeCamera::_dynReflex;
EagleEyeCamera::EagleEyeCamera(const wstring& name, SceneManager* originSceneMgr)
	: OrbitCamera(name, originSceneMgr)
	, _zoomFactor(1.0f)
{
	const Dimension2di& size = Root::getSingleton().getActiveDevice()->getScreenSize();
	_nearWindowHeight = size._width;
	_farWindowHeight = size._height;
	_nearZ = -10000.0f;
	_farZ = 10000.0f;
	_zoomSpeed[0] = _zoomSpeed[1] = 0.1f;
	_arcball.updateScale(_zoomFactor, _zoomFactor, _zoomFactor);
}

EagleEyeCamera::~EagleEyeCamera()
{
}

void EagleEyeCamera::update(float dt)
{
	if (Root::getSingleton().getCurSceneMgr()->getActiveCamera() != this)
		return;
	if (_direction.checkState(EDIRECTION_UPWARD))
		fly(_moveSpeed[0] * dt);
	if (_direction.checkState(EDIRECTION_DOWNWARD))
		fly(-_moveSpeed[0] * dt);
	if (_direction.checkState(EDIRECTION_LEFTWARD))
		strafe(-_moveSpeed[0] * dt);
	if (_direction.checkState(EDIRECTION_RIGHTWARD))
		strafe(_moveSpeed[0] * dt);
	OrbitCamera::update(dt);
}

void EagleEyeCamera::setZoomFactor(float zoom)
{
	_zoomFactor = zoom;
	_arcball.updateScale(_zoomFactor, _zoomFactor, _zoomFactor);
}

void EagleEyeCamera::setLens(float widthOfViewVolume, float heightOfViewVolume, float zn, float zf)
{
	_arcball.setBounds(widthOfViewVolume, heightOfViewVolume);
	_nearWindowHeight = widthOfViewVolume;
	_farWindowHeight = heightOfViewVolume;
	_nearZ = zn;
	_farZ = zf;
	_viewArea.getTransform(Frustum::EFT_PROJECTION).buildProjectionMatrixOrthoRH(_nearWindowHeight, _farWindowHeight, _nearZ, _farZ);
}

float EagleEyeCamera::getFovY() const
{
	return 180.0f;
}

float EagleEyeCamera::getFovX() const
{
	return 180.0f;
}

float EagleEyeCamera::getNearWindowWidth() const
{
	return _nearWindowHeight;
}

float EagleEyeCamera::getNearWindowHeight() const
{
	return _farWindowHeight;
}

float EagleEyeCamera::getFarWindowWidth() const
{
	return _nearWindowHeight;
}

bool EagleEyeCamera::touchBegan(const Touch& touch, EMouseKeys key)
{
	return false;
}

void EagleEyeCamera::touchMoved(const Touch& touch, EMouseKeys key)
{
	switch (key)
	{
	case EMK_RIGHT:
		_arcball.updateTranslate(touch.getDelta()._x * _moveSpeed[0], -touch.getDelta()._y * _moveSpeed[0], 0);
		break;
	}
}

void EagleEyeCamera::touchEnded(const Touch& touch, EMouseKeys key)
{
	// do nothing.
}

bool EagleEyeCamera::touchWheel(const Touch& touch, float wheel)
{
	_zoomFactor += wheel * _zoomSpeed[0];
	if (_zoomFactor <= 0.01f)
		_zoomFactor -= wheel * _zoomSpeed[0];
	_arcball.updateScale(_zoomFactor, _zoomFactor, _zoomFactor);
	return true;
}

bool EagleEyeCamera::keyClicked(const tagKeyInput& keyInput)
{
	OrbitCamera::keyClicked(keyInput);

	bool ret = false;
	unsigned int uCurDire = 0;
	switch (keyInput.Key)
	{
	case KEY_KEY_W: uCurDire = EDIRECTION_DOWNWARD; ret = true; break;
	case KEY_KEY_S: uCurDire = EDIRECTION_UPWARD; ret = true; break;
	case KEY_KEY_A: uCurDire = EDIRECTION_RIGHTWARD; ret = true; break;
	case KEY_KEY_D: uCurDire = EDIRECTION_LEFTWARD; ret = true; break;
	default: _direction.clearState(); break;
	}

	if (keyInput.PressedDown)
		_direction.addState(uCurDire);
	else
		_direction.eraseState(uCurDire);

	return ret;
}

ViewMementoPtr EagleEyeCamera::createViewMemento()
{
	return ViewMementoPtr(new ViewMementoEagleEye());
}

void EagleEyeCamera::serialize(XMLElement* outXml)
{
	OrbitCamera::serialize(outXml);
	outXml->SetAttribute("zoomFactor", _zoomFactor);
}

bool EagleEyeCamera::deserialize(XMLElement* inXml)
{
	if (!OrbitCamera::deserialize(inXml))
		return false;
	inXml->QueryFloatAttribute("zoomFactor", &_zoomFactor);
	setZoomFactor(_zoomFactor);
	return true;
}

NS_VAYO3D_END
