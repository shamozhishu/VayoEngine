#include "stdafx.h"
#include "ModelApp.h"

ModelApp::ModelApp(const Root::Config& config)
	: App(config)
	, m_cam(NULL)
{
}

bool ModelApp::startup()
{
	Log::print(ELL_INFORMATION, "Model Builder ��������");

	// ���س���
	SceneManager* pCurSceneMgr = Root::getSingleton().createSceneMgr(L"SimpleScene");
	if (!pCurSceneMgr->loadScene(L"SimpleScene.scene"))
		return false;

	// ��ȡ�����
	m_cam = pCurSceneMgr->findObject<OrbitCamera>(L"��������");

	// �����������
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
