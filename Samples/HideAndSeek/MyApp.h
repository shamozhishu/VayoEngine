#pragma once

#define VAYO_ENABLE_MEM_LEAK_CHECK
#include "Vayo.h"
using namespace Vayo;

#define HAS_FPS_CAMERA    (0)
#define HAS_TESTING_BOARD (0)
#define HAS_TESTING_MODEL (0)

class ModelViewer : public App, public Singleton<ModelViewer>
{
public:
	ModelViewer(const Root::Config& config);
	bool startup();
	void cleanup();
	bool keyClicked(const tagKeyInput& keyInput);

private:
	class TestModel* m_testModel;
	class BoxModel* m_boxModel;
	PROPERTY_R(FPSCamera*, m_cameraFPS, FPSCamera)
	PROPERTY_R(OrbitCamera*, m_cameraOrbit, OrbitCamera)
};
