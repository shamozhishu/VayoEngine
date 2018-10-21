#include "VayoCamera.h"
#include "VayoRoot.h"
#include "VayoDevice.h"
#include "VayoRenderSystem.h"
#include "VayoSceneNode.h"
#include "VayoSceneManager.h"
#include "tinyxml2/tinyxml2.h"
#include "VayoUtils.h"

NS_VAYO_BEGIN

class ViewMementoFPS : public Camera::ViewMemento
{
public:
	vector<SpatialInfo> _placement;
	Matrix4x4 _affector;
};

class ViewMementoOrbit : public ViewMementoFPS
{
public:
	Matrix4x4 _transform;
	Matrix4x4 _thisRot;
	Matrix4x4 _lastRot;
};

class ViewMementoEagleEye : public ViewMementoFPS
{
public:
	float     _zoomFactor;
	Matrix4x4 _transform;
};

Camera::Camera()
	: _right(1.0f, 0.0f, 0.0f)
	, _up(0.0f, 1.0f, 0.0f)
	, _look(0.0f, 0.0f, -1.0f)
	, _needUpdate(true)
{
	Device* pDevice = Root::getSingleton().getDevice();
	const Dimension2di& size = pDevice->getScreenSize();
	_aspect = (float)size._width / size._height;
	_nearZ = 1.0f;
	_farZ = 1000.0f;
}

Camera::~Camera()
{
	map<wstring, ViewMemento*>::iterator it = _viewMementoPool.begin();
	for (; it != _viewMementoPool.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}

	_viewMementoPool.clear();
	Root::getSingleton().getTouchDispatcher()->removeTouchDelegate(this);
	Root::getSingleton().getKeypadDispatcher()->removeKeypadDelegate(this);
}

void Camera::refresh()
{
	if (_needUpdate)
	{
		updateViewTransform();
		_needUpdate = false;
	}

	rebuildViewArea();
	recalculateViewArea();
	Root::getSingleton().getActiveRenderer()->setTransform(ETS_VIEW, getView());
	Root::getSingleton().getActiveRenderer()->setTransform(ETS_PROJECTION, getProj());
}

void Camera::setNeedUpdate(bool isUpdate)
{
	_needUpdate = isUpdate;
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

void Camera::rebuildViewArea()
{
	_viewArea.getTransform(Frustum::EFT_VIEW) = _view * _affector;
}

void Camera::recalculateViewArea()
{
	getWorldPos(_viewArea._cameraPosition);
	Matrix4x4 m = _viewArea.getTransform(Frustum::EFT_PROJECTION) * _viewArea.getTransform(Frustum::EFT_VIEW);
	_viewArea.setFrom(m);
}

//////////////////////////////////////////////////////////////////////////
FPSCamera::FPSCamera(const wstring& name)
	: MovableObject(name)
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

void FPSCamera::createViewMemento(const wstring& name)
{
	if (name != L"")
	{
		ViewMementoFPS* viewPtr = new ViewMementoFPS();
		viewPtr->_position = _position;
		viewPtr->_right = _right;
		viewPtr->_up = _up;
		viewPtr->_look = _look;
		viewPtr->_affector = _affector;

		SpatialInfo place;
		Node* pParent = getParentNode();
		while (pParent)
		{
			place._translation = pParent->getPosition();
			place._rotation = pParent->getRotation();
			place._scale = pParent->getScale();
			viewPtr->_placement.push_back(place);
			pParent = pParent->getParent();
		}

		_viewMementoPool[name] = viewPtr;
	}
}

void FPSCamera::restoreViewMemento(const wstring& name)
{
	map<wstring, ViewMemento*>::iterator it = _viewMementoPool.find(name);
	if (it != _viewMementoPool.end())
	{
		ViewMementoFPS* viewPtr = dynamic_cast<ViewMementoFPS*>(it->second);

		if (viewPtr)
		{
			_position = viewPtr->_position;
			_right = viewPtr->_right;
			_up = viewPtr->_up;
			_look = viewPtr->_look;
			_affector = viewPtr->_affector;

			Node* pParent = getParentNode();
			if (pParent)
			{
				vector<SpatialInfo>::iterator itor = viewPtr->_placement.begin();
				for (; itor != viewPtr->_placement.end() && pParent; ++itor)
				{
					const SpatialInfo& place = *itor;
					pParent->setPosition(place._translation);
					pParent->setRotation(place._rotation);
					pParent->setScale(place._scale);
					pParent = pParent->getParent();
				}
			}

			_needUpdate = true;
		}

		SAFE_DELETE(it->second);
		_viewMementoPool.erase(it);
	}
}

void FPSCamera::rebuildViewArea()
{
	SceneNode* pParent = getParentNode();
	if (pParent)
		_viewArea.getTransform(Frustum::EFT_VIEW) = _view * pParent->getAbsTransformation() * _affector;
	else
		Camera::rebuildViewArea();
}

bool FPSCamera::parseXML(XMLElement* xml)
{
	if (!xml)
		return false;

	_moveSpeed[0] = _moveSpeed[1] = xml->FloatAttribute("moveSpeed");

	Vector3df position(0, 0, 0), target(0, 0, -1), worldUp(0, 1, 0);
	vector<string> container;
	string strTemp = xml->Attribute("position");
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		position.set(x, y, z);
	}

	strTemp = xml->Attribute("target");
	container.clear();
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		target.set(x, y, z);
	}

	strTemp = xml->Attribute("worldUp");
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

	float fovY = xml->FloatAttribute("fovY");
	float zn = xml->FloatAttribute("zn");
	float zf = xml->FloatAttribute("zf");

	setLens(fovY, getAspect(), zn, zf);

	return true;
}

//////////////////////////////////////////////////////////////////////////
OrbitCamera::OrbitCamera(const wstring& name)
	: MovableObject(name)
	, _arcball(0, 0)
{
	_moveSpeed[0] = _moveSpeed[1] = _zoomSpeed[0] = _zoomSpeed[1] = 5.0f;
	const Dimension2di& size = Root::getSingleton().getDevice()->getScreenSize();
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
	Dimension2di size = Root::getSingleton().getDevice()->getScreenSize();
	_arcball.setBounds(size._width, size._height);
}

void OrbitCamera::lookAt(const Vector3df& pos, const Vector3df& target /*= Vector3df::Origin*/, const Vector3df& worldUp /*= Vector3df::YAxis*/)
{
	float zpos = max_(max_(pos._x, pos._y), pos._z);
	_position.set(0, 0, zpos);
	_affector.buildCameraLookAtMatrixRH(pos, target, worldUp);
	_affector.setTranslation(Vector3df::Origin);
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

void OrbitCamera::createViewMemento(const wstring& name)
{
	if (name != L"")
	{
		ViewMementoOrbit* viewPtr = new ViewMementoOrbit();
		viewPtr->_position = _position;
		viewPtr->_right = _right;
		viewPtr->_up = _up;
		viewPtr->_look = _look;
		viewPtr->_affector = _affector;
		viewPtr->_transform = _arcball._transform;
		viewPtr->_lastRot = _arcball._lastRot;
		viewPtr->_thisRot = _arcball._thisRot;

		SpatialInfo place;
		Node* pParent = getParentNode();
		while (pParent)
		{
			place._translation = pParent->getPosition();
			place._rotation = pParent->getRotation();
			place._scale = pParent->getScale();
			viewPtr->_placement.push_back(place);
			pParent = pParent->getParent();
		}

		_viewMementoPool[name] = viewPtr;
	}
}

void OrbitCamera::restoreViewMemento(const wstring& name)
{
	map<wstring, ViewMemento*>::iterator it = _viewMementoPool.find(name);
	if (it != _viewMementoPool.end())
	{
		ViewMementoOrbit* viewPtr = dynamic_cast<ViewMementoOrbit*>(it->second);

		if (viewPtr)
		{
			_position = viewPtr->_position;
			_right = viewPtr->_right;
			_up = viewPtr->_up;
			_look = viewPtr->_look;
			_affector = viewPtr->_affector;
			_arcball._transform = viewPtr->_transform;
			_arcball._lastRot = viewPtr->_lastRot;
			_arcball._thisRot = viewPtr->_thisRot;

			Node* pParent = getParentNode();
			if (pParent)
			{
				vector<SpatialInfo>::iterator itor = viewPtr->_placement.begin();
				for (; itor != viewPtr->_placement.end() && pParent; ++itor)
				{
					const SpatialInfo& place = *itor;
					pParent->setPosition(place._translation);
					pParent->setRotation(place._rotation);
					pParent->setScale(place._scale);
					pParent = pParent->getParent();
				}
			}

			_needUpdate = true;
		}

		SAFE_DELETE(it->second);
		_viewMementoPool.erase(it);
	}
}

void OrbitCamera::rebuildViewArea()
{
	SceneNode* pParent = getParentNode();
	if (pParent)
		_viewArea.getTransform(Frustum::EFT_VIEW) = _view * pParent->getAbsTransformation() * _arcball._transform * _affector;
	else
		_viewArea.getTransform(Frustum::EFT_VIEW) = _view * _arcball._transform * _affector;
}

bool OrbitCamera::parseXML(XMLElement* xml)
{
	if (!xml)
		return false;

	_moveSpeed[0] = _moveSpeed[1] = xml->FloatAttribute("moveSpeed");
	_zoomSpeed[0] = _zoomSpeed[1] = xml->FloatAttribute("zoomSpeed");
	
	Vector3df position(0, 0, 0), target(0, 0, -1), worldUp(0, 1, 0);
	vector<string> container;
	string strTemp = xml->Attribute("position");
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		position.set(x, y, z);
	}

	strTemp = xml->Attribute("target");
	container.clear();
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		target.set(x, y, z);
	}

	strTemp = xml->Attribute("worldUp");
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

	float fovY = xml->FloatAttribute("fovY");
	float zn = xml->FloatAttribute("zn");
	float zf = xml->FloatAttribute("zf");

	setLens(fovY, getAspect(), zn, zf);

	return true;
}

//////////////////////////////////////////////////////////////////////////
EagleEyeCamera::EagleEyeCamera(const wstring& name)
	: OrbitCamera(name)
	, _zoomFactor(1.0f)
{
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
	Dimension2di size = Root::getSingleton().getDevice()->getScreenSize();
	_arcball.setBounds(size._width, size._height);
	float w = size._width;
	float h = size._height;
	if (0 == size._height)
		h = 1.0f;
	if (!equals(widthOfViewVolume, 180.0f) || !equals(w / h, heightOfViewVolume))
	{
		_nearWindowHeight = widthOfViewVolume;
		_farWindowHeight = heightOfViewVolume;
	}
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

float EagleEyeCamera::getFarWindowHeight() const
{
	return _farWindowHeight;
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

void EagleEyeCamera::createViewMemento(const wstring& name)
{
	if (name != L"")
	{
		ViewMementoEagleEye* viewPtr = new ViewMementoEagleEye();
		viewPtr->_position = _position;
		viewPtr->_right = _right;
		viewPtr->_up = _up;
		viewPtr->_look = _look;
		viewPtr->_affector = _affector;
		viewPtr->_zoomFactor = _zoomFactor;
		viewPtr->_transform = _arcball._transform;

		SpatialInfo place;
		Node* pParent = getParentNode();
		while (pParent)
		{
			place._translation = pParent->getPosition();
			place._rotation = pParent->getRotation();
			place._scale = pParent->getScale();
			viewPtr->_placement.push_back(place);
			pParent = pParent->getParent();
		}

		_viewMementoPool[name] = viewPtr;
	}
}

void EagleEyeCamera::restoreViewMemento(const wstring& name)
{
	map<wstring, ViewMemento*>::iterator it = _viewMementoPool.find(name);
	if (it != _viewMementoPool.end())
	{
		ViewMementoEagleEye* viewPtr = dynamic_cast<ViewMementoEagleEye*>(it->second);

		if (viewPtr)
		{
			_position = viewPtr->_position;
			_right = viewPtr->_right;
			_up = viewPtr->_up;
			_look = viewPtr->_look;
			_affector = viewPtr->_affector;
			_zoomFactor = viewPtr->_zoomFactor;
			_arcball._transform = viewPtr->_transform;

			Node* pParent = getParentNode();
			if (pParent)
			{
				vector<SpatialInfo>::iterator itor = viewPtr->_placement.begin();
				for (; itor != viewPtr->_placement.end() && pParent; ++itor)
				{
					const SpatialInfo& place = *itor;
					pParent->setPosition(place._translation);
					pParent->setRotation(place._rotation);
					pParent->setScale(place._scale);
					pParent = pParent->getParent();
				}
			}

			_needUpdate = true;
		}

		SAFE_DELETE(it->second);
		_viewMementoPool.erase(it);
	}
}

NS_VAYO_END
