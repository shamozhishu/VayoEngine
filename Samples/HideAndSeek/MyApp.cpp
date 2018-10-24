#include "MyApp.h"
#include "pages.h"
#include "models.h"

ModelViewer::ModelViewer(const Root::Config& config)
	: App(config)
	, m_cameraFPS(NULL)
	, m_cameraOrbit(NULL)
	, m_testModel(NULL)
	, m_boxModel(NULL)
{
}

bool ModelViewer::startup()
{
	Log::print(ELL_INFORMATION, "Model Viewer ��������");

	// ���س���
	SceneManager* pCurSceneMgr = Root::getSingleton().createSceneMgr(L"���Գ���");
	if (!pCurSceneMgr->loadScene(L"���Գ���.scene"))
		return false;

	// ��ȡ�����
	m_cameraFPS = pCurSceneMgr->findObject<FPSCamera>(L"��һ�˳������");
	m_cameraOrbit = pCurSceneMgr->findObject<OrbitCamera>(L"��������");

#if HAS_TESTING_BOARD
	{
		TessGridBuilder gridBuilder(L"����.tessgrid");
	}
#endif
#if HAS_TESTING_MODEL
	m_testModel = new TestModel(pCurSceneMgr);
#endif
	m_boxModel = new BoxModel(pCurSceneMgr);

	{
		TessGridHandler gridder;
		gridder.parseTessgridFile(L"������_J599I_26FG41SHN.tessgrid");
		gridder.parseTessgridFile(L"������_J599I_26FH55PHN.tessgrid");
		gridder.parseTessgridFile(L"������_J599I_26FJ61PA.tessgrid");
		gridder.parseTessgridFile(L"������_J599I_26FJ61PN.tessgrid");
		gridder.parseTessgridFile(L"������_J599I_26FJ61SA.tessgrid");
		gridder.parseTessgridFile(L"������_J599I_26FJ61SHN.tessgrid");
		gridder.parseTessgridFile(L"������_J599I_26FJ61SN.tessgrid");
		gridder.parseTessgridFile(L"β��_J1784A_18B_21N08.tessgrid");
		gridder.parseTessgridFile(L"β��_J1784A_18B_23N09.tessgrid");
		gridder.parseTessgridFile(L"β��_J1784A_18B_25N10.tessgrid");
		gridder.parseTessgridFile(L"����_AF_250.tessgrid");
		gridder.parseTessgridFile(L"����_609C55A0812_22_6.tessgrid");
		gridder.parseTessgridFile(L"star.tessgrid");

		TessGridBuilder builder;
		builder.setProp(L"testmodel", L"examples/sphere");
		builder.setPlace(TessGridBuilder::EP_XY, Vector3df(0,0,2), Vector3df(0,0,0), Vector3df(1,1,1));
		builder.setTopCap(-1, Vector3df(0,0,1), L"examples/sphere", false);
		builder.setBottomCap(-1, Vector3df(0, 0, -1), L"examples/sphere", true);
		builder.addCircle(25.68f, Vector2df(0, 0));
		builder.addCircle(5.68f, Vector2df(5, 3));
		builder.addCircle(5.68f, Vector2df(-5, -5));
		builder.addCircle(5.68f, Vector2df(-15, -15));
		builder.beginAddPoly();
		builder.addPolyPoint(Vector2df(10, 5), 0);
		builder.addPolyPoint(Vector2df(-10, 5), 90, 180);
		builder.addPolyPoint(Vector2df(-10, -5), 0);
		builder.addPolyPoint(Vector2df(10, -5), 0);
		builder.endAddPoly();
		builder.beginAddStretBody(0, 3, L"examples/���Ӳ���", false);
		builder.addSingleStret(Vector3df(0, 0, -4), Vector3df(0, 0, 0), Vector3df(1, 1, 1));
		builder.endAddStretBody();
		builder.beginAddStretBody(4, L"examples/���Ӳ���", true);
		builder.addSingleStret(Vector3df(0, 0, -4), Vector3df(0, 0, 0), Vector3df(1, 1, 1));
		builder.endAddStretBody();
		gridder.parseTessgridFile(builder.getStream());
		builder.save(Root::getSingleton().getConfigManager()->getSceneConfig().ModelsPath + L"testmodel2");
	}

	// �����������
	pCurSceneMgr->setActiveCamera(m_cameraOrbit);

	// ��ʼ��ͼ���û�����
	new UIInterface();
	if (NULL == UIInterface::getSingletonPtr() || !UIInterface::getSingletonPtr()->init())
		return false;

	return true;
}

void ModelViewer::cleanup()
{
	delete UIInterface::getSingletonPtr();
	SAFE_DELETE(m_testModel);
	SAFE_DELETE(m_boxModel);
}

bool ModelViewer::keyClicked(const tagKeyInput& keyInput)
{
#if HAS_FPS_CAMERA
	if (keyInput.PressedDown)
		return false;

	switch (keyInput.Key)
	{
	case KEY_SPACE:
		if (Root::getSingleton().getCurSceneMgr()->getActiveCamera() == m_cameraFPS)
		{
			Root::getSingleton().getCurSceneMgr()->setActiveCamera(m_cameraOrbit);
			Log::wprint(ELL_INFORMATION, L"�����������");
		}
		else
		{
			Root::getSingleton().getCurSceneMgr()->setActiveCamera(m_cameraFPS);
			Log::wprint(ELL_INFORMATION, L"�����һ�˳������");
		}
		return true;
	}
#endif
	return false;
}
