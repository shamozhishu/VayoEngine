#include "stdafx.h"
#include "TessgridApp.h"

TessgridApp::TessgridApp(const Root::Config& config)
	: App(config)
	, m_cam(NULL)
{
}

bool TessgridApp::startup()
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

void TessgridApp::cleanup()
{
}

bool TessgridApp::keyClicked(const tagKeyInput& keyInput)
{
	return false;
}
