#include "Vayo3dViewMemento.h"
#include "Vayo3dCamera.h"
#include "Vayo3dSceneNode.h"

NS_VAYO3D_BEGIN

void ViewMemento::storage(Camera* camera)
{
	_position = camera->_position;
	_right = camera->_right;
	_up = camera->_up;
	_look = camera->_look;
	_affector = camera->_affector;
}

void ViewMemento::recover(Camera* camera)
{
	camera->_position = _position;
	camera->_right = _right;
	camera->_up = _up;
	camera->_look = _look;
	camera->_affector = _affector;
}

//////////////////////////////////////////////////////////////////////////
void ViewMementoFPS::storage(Camera* camera)
{
	ViewMemento::storage(camera);
	FPSCamera* cameraFPS = dynamic_cast<FPSCamera*>(camera);
	if (cameraFPS)
	{
		setSpatialInfo(cameraFPS->getParentNode());
		_moveSpeed = cameraFPS->_moveSpeed[1];
	}
}

void ViewMementoFPS::recover(Camera* camera)
{
	ViewMemento::recover(camera);
	FPSCamera* cameraFPS = dynamic_cast<FPSCamera*>(camera);
	if (cameraFPS)
	{
		getSpatialInfo(cameraFPS->getParentNode());
		cameraFPS->_moveSpeed[0] = cameraFPS->_moveSpeed[1] = _moveSpeed;
	}
}

void ViewMementoFPS::setSpatialInfo(Node* pParent)
{
	SpatialInfo place;
	while (pParent)
	{
		place._translation = pParent->getPosition();
		place._rotation = pParent->getRotation();
		place._scale = pParent->getScale();
		_placement.push_back(place);
		pParent = pParent->getParent();
	}
}

void ViewMementoFPS::getSpatialInfo(Node* pParent)
{
	if (pParent)
	{
		vector<SpatialInfo>::iterator itor = _placement.begin();
		for (; itor != _placement.end() && pParent; ++itor)
		{
			const SpatialInfo& place = *itor;
			pParent->setPosition(place._translation);
			pParent->setRotation(place._rotation);
			pParent->setScale(place._scale);
			pParent = pParent->getParent();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void ViewMementoOrbit::storage(Camera* camera)
{
	ViewMemento::storage(camera);
	OrbitCamera* cameraOrbit = dynamic_cast<OrbitCamera*>(camera);
	if (cameraOrbit)
	{
		ViewMementoFPS::setSpatialInfo(cameraOrbit->getParentNode());
		_moveSpeed = cameraOrbit->_moveSpeed[1];
		_zoomSpeed = cameraOrbit->_zoomSpeed[1];
		_thisRot = cameraOrbit->_arcball._thisRot;
		_lastRot = cameraOrbit->_arcball._lastRot;
		_transform = cameraOrbit->_arcball._transform;
	}
}

void ViewMementoOrbit::recover(Camera* camera)
{
	ViewMemento::recover(camera);
	OrbitCamera* cameraOrbit = dynamic_cast<OrbitCamera*>(camera);
	if (cameraOrbit)
	{
		ViewMementoFPS::getSpatialInfo(cameraOrbit->getParentNode());
		cameraOrbit->_moveSpeed[0] = cameraOrbit->_moveSpeed[1] = _moveSpeed;
		cameraOrbit->_zoomSpeed[0] = cameraOrbit->_zoomSpeed[1] = _zoomSpeed;
		cameraOrbit->_arcball._thisRot = _thisRot;
		cameraOrbit->_arcball._lastRot = _lastRot;
		cameraOrbit->_arcball._transform = _transform;
	}
}

//////////////////////////////////////////////////////////////////////////
void ViewMementoEagleEye::storage(Camera* camera)
{
	ViewMemento::storage(camera);
	EagleEyeCamera* cameraEagleEye = dynamic_cast<EagleEyeCamera*>(camera);
	if (cameraEagleEye)
	{
		ViewMementoFPS::setSpatialInfo(cameraEagleEye->getParentNode());
		_moveSpeed = cameraEagleEye->_moveSpeed[1];
		_zoomSpeed = cameraEagleEye->_zoomSpeed[1];
		_zoomFactor = cameraEagleEye->_zoomFactor;
		_transform = cameraEagleEye->_arcball._transform;
	}
}

void ViewMementoEagleEye::recover(Camera* camera)
{
	ViewMemento::recover(camera);
	EagleEyeCamera* cameraEagleEye = dynamic_cast<EagleEyeCamera*>(camera);
	if (cameraEagleEye)
	{
		ViewMementoFPS::getSpatialInfo(cameraEagleEye->getParentNode());
		cameraEagleEye->_moveSpeed[0] = cameraEagleEye->_moveSpeed[1] = _moveSpeed;
		cameraEagleEye->_zoomSpeed[0] = cameraEagleEye->_zoomSpeed[1] = _zoomSpeed;
		cameraEagleEye->_zoomFactor = _zoomFactor;
		cameraEagleEye->_arcball._transform = _transform;
	}
}

NS_VAYO3D_END
