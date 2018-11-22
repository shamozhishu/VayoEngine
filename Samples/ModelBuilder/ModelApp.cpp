#include "stdafx.h"
#include "ModelApp.h"

ModelApp::ModelApp(const Root::Config& config)
	: App(config)
	, m_cam(NULL)
{
}

bool ModelApp::startup()
{
	Log::print(ELL_INFORMATION, "Model Builder 程序启动");

	// 加载场景
	SceneManager* pCurSceneMgr = Root::getSingleton().createSceneMgr(L"SimpleScene");
	if (!pCurSceneMgr->loadScene(L"SimpleScene.scene"))
		return false;

	// 获取摄像机
	m_cam = pCurSceneMgr->findObject<OrbitCamera>(L"轨道摄像机");

	// 激活轨道摄像机
	pCurSceneMgr->setActiveCamera(m_cam);

	return true;
}

void ModelApp::cleanup()
{
}

bool ModelApp::keyClicked(const tagKeyInput& keyInput)
{
	return false;
}
