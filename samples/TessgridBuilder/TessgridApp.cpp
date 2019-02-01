#include "stdafx.h"
#include "TessgridApp.h"

TessgridApp::TessgridApp(Root::Config* config)
	: App(config)
	, m_cam(NULL)
{
}

bool TessgridApp::startup()
{
	Log::print(ELL_INFORMATION, "Model Builder 程序启动");

	// 创建场景
	SceneManager* pCurSceneMgr = Root::getSingleton().createSceneMgr(L"TessgridScene");
	Root::getSingleton().bootFrame(NULL, pCurSceneMgr->getName());

	// 创建灯光
	SceneNode* pSeneNode = pCurSceneMgr->getRootSceneNode()->createChildSceneNode(L"灯光场景节点");
	DirectionalLight* pDireLight = pCurSceneMgr->createObject<DirectionalLight>(L"主方向光");
	pSeneNode->attachObject(pDireLight);
	pDireLight->setAmbient(128, 128, 128, 255);
	pDireLight->setDiffuse(255, 255, 255, 255);
	pDireLight->setSpecular(204, 204, 204, 255);
	pDireLight->setDirection(-0.57735f, 0.57735f, 0.57735f);
	pDireLight = pCurSceneMgr->createObject<DirectionalLight>(L"辅助方向光1");
	pSeneNode->attachObject(pDireLight);
	pDireLight->setAmbient(0, 0, 0, 255);
	pDireLight->setDiffuse(102, 102, 102, 255);
	pDireLight->setSpecular(51, 51, 51, 255);
	pDireLight->setDirection(0.707f, -0.707f, 0.0f);
	pDireLight = pCurSceneMgr->createObject<DirectionalLight>(L"辅助方向光2");
	pSeneNode->attachObject(pDireLight);
	pDireLight->setAmbient(0, 0, 0, 255);
	pDireLight->setDiffuse(102, 102, 102, 255);
	pDireLight->setSpecular(51, 51, 51, 255);
	pDireLight->setDirection(0.0f, 0.0f, -1.0f);

	// 创建轨道摄像机
	m_cam = pCurSceneMgr->createObject<OrbitCamera>(L"轨道摄像机");
	pCurSceneMgr->getRootSceneNode()->attachObject(m_cam);
	Dimension2di winSize = Root::getSingleton().getActiveDevice()->getScreenSize();
	m_cam->setLens(45.0f, (float)winSize._width/winSize._height, 1.0f, 1000.0f);
	m_cam->setMoveSpeed(20);

	// 激活轨道摄像机
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
