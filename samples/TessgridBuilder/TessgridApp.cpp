#include "stdafx.h"
#include "TessgridApp.h"

TessgridApp::TessgridApp(Root::Config* config)
	: App(config)
	, m_cam(NULL)
{
}

bool TessgridApp::startup()
{
	Log::print(ELL_INFORMATION, "Model Builder ��������");

	// ��������
	SceneManager* pCurSceneMgr = Root::getSingleton().createSceneMgr(L"TessgridScene");
	Root::getSingleton().bootFrame(NULL, pCurSceneMgr->getName());

	// �����ƹ�
	SceneNode* pSeneNode = pCurSceneMgr->getRootSceneNode()->createChildSceneNode(L"�ƹⳡ���ڵ�");
	DirectionalLight* pDireLight = pCurSceneMgr->createObject<DirectionalLight>(L"�������");
	pSeneNode->attachObject(pDireLight);
	pDireLight->setAmbient(128, 128, 128, 255);
	pDireLight->setDiffuse(255, 255, 255, 255);
	pDireLight->setSpecular(204, 204, 204, 255);
	pDireLight->setDirection(-0.57735f, 0.57735f, 0.57735f);
	pDireLight = pCurSceneMgr->createObject<DirectionalLight>(L"���������1");
	pSeneNode->attachObject(pDireLight);
	pDireLight->setAmbient(0, 0, 0, 255);
	pDireLight->setDiffuse(102, 102, 102, 255);
	pDireLight->setSpecular(51, 51, 51, 255);
	pDireLight->setDirection(0.707f, -0.707f, 0.0f);
	pDireLight = pCurSceneMgr->createObject<DirectionalLight>(L"���������2");
	pSeneNode->attachObject(pDireLight);
	pDireLight->setAmbient(0, 0, 0, 255);
	pDireLight->setDiffuse(102, 102, 102, 255);
	pDireLight->setSpecular(51, 51, 51, 255);
	pDireLight->setDirection(0.0f, 0.0f, -1.0f);

	// ������������
	m_cam = pCurSceneMgr->createObject<OrbitCamera>(L"��������");
	pCurSceneMgr->getRootSceneNode()->attachObject(m_cam);
	Dimension2di winSize = Root::getSingleton().getActiveDevice()->getScreenSize();
	m_cam->setLens(45.0f, (float)winSize._width/winSize._height, 1.0f, 1000.0f);
	m_cam->setMoveSpeed(20);

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
