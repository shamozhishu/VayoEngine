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
	m_cameraEagleEye = pCurSceneMgr->findObject<EagleEyeCamera>(L"ӥ�������");

#if HAS_TESTING_MODEL
	m_testModel = new TestModel(pCurSceneMgr);
#endif
	m_boxModel = new BoxModel(pCurSceneMgr);

	{
		TessGridHandler gridder;
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
		builder.save(L"testmodel2", false);
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
	if (keyInput.PressedDown)
		return false;

	switch (keyInput.Key)
	{
	case KEY_F1:
		Root::getSingleton().getCurSceneMgr()->setActiveCamera(m_cameraFPS);
		Log::wprint(ELL_INFORMATION, L"�����һ�˳������");
		return true;
	case KEY_F2:
		Root::getSingleton().getCurSceneMgr()->setActiveCamera(m_cameraOrbit);
		Log::wprint(ELL_INFORMATION, L"�����������");
		return true;
	case KEY_F3:
		Root::getSingleton().getCurSceneMgr()->setActiveCamera(m_cameraEagleEye);
		Log::wprint(ELL_INFORMATION, L"����ӥ�������");
		return true;
	}

	return false;
}
