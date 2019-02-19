#pragma once

#define VAYO_ENABLE_MEM_LEAK_CHECK
#include "Vayo3d.h"
using namespace Vayo;
using namespace Vayo::_3d;

#define HAS_TESTING_MODEL (0)

class ModelViewer : public App, public Singleton<ModelViewer>
{
public:
	ModelViewer();
	bool startup();
	void cleanup();
	bool keyClicked(const tagKeyInput& keyInput);

private:
	class TestModel* m_testModel;
	class BoxModel* m_boxModel;
	PROPERTY_R(FPSCamera*, m_cameraFPS, FPSCamera)
	PROPERTY_R(OrbitCamera*, m_cameraOrbit, OrbitCamera)
	PROPERTY_R(EagleEyeCamera*, m_cameraEagleEye, EagleEyeCamera)
};
