#include "pages.h"
#include "models.h"

UIInterface::UIInterface()
	: m_pTestPage(NULL)
	, m_pAnotherPage(NULL)
	, m_pModelPage(NULL)
{
}

UIInterface::~UIInterface()
{
	SAFE_DELETE(m_pTestPage);
	SAFE_DELETE(m_pAnotherPage);
	SAFE_DELETE(m_pModelPage);
}

bool UIInterface::init()
{
	m_pTestPage = new TestPage;
	m_pAnotherPage = new AnotherPage;
	m_pModelPage = new ModelPage;
	m_pTestPage->open(false);
	m_pAnotherPage->open(false);
	m_pModelPage->open(false);
	return true;
}

//////////////////////////////////////////////////////////////////////////
TestPage::TestPage()
	: UIDialog(L"testpage.xml")
{
}

TestPage::~TestPage()
{
}

void TestPage::open(bool bBlockMode/*=true*/)
{
	UIDialog::open(bBlockMode);
	showWidget(false);
}

//////////////////////////////////////////////////////////////////////////
AnotherPage::AnotherPage()
	: UIDialog(L"anotherpage.xml")
{
	m_pCtrl = getCtrl(1001);
}

AnotherPage::~AnotherPage()
{
}

void AnotherPage::open(bool bBlockMode/*=true*/)
{
	UIDialog::open(bBlockMode);
	showWidget(false);
}

void AnotherPage::setAttribText(const wstring& attribText)
{
	m_pCtrl->setText(attribText);
}

//////////////////////////////////////////////////////////////////////////
ModelPage::ModelPage()
	: UIDialog(L"modelpage.xml")
{
	m_btn1 = (UIButton*)getCtrl(1);
	m_btn2 = (UIButton*)getCtrl(2);
	m_btn3 = (UIButton*)getCtrl(3);
	m_btn4 = (UIButton*)getCtrl(4);
	m_btn5 = (UIButton*)getCtrl(5);
	m_btn6 = (UIButton*)getCtrl(6);
	m_btn7 = (UIButton*)getCtrl(7);
	m_btn8 = (UIButton*)getCtrl(8);
	m_btn9 = (UIButton*)getCtrl(9);
	m_btn10 = (UIButton*)getCtrl(10);
	m_btn11 = (UIButton*)getCtrl(11);
	m_btn12 = (UIButton*)getCtrl(12);
	m_btn13 = (UIButton*)getCtrl(13);
	m_btn14 = (UIButton*)getCtrl(14);
	m_btn15 = (UIButton*)getCtrl(15);

	m_pCurSceneMgr = Root::getSingleton().getCurSceneMgr();

	m_pObj1 = m_pCurSceneMgr->findObject<ManualObject>(m_btn1->getText());
	m_pObj2 = m_pCurSceneMgr->findObject<ManualObject>(m_btn2->getText());
	m_pObj3 = m_pCurSceneMgr->findObject<ManualObject>(m_btn3->getText());
	m_pObj4 = m_pCurSceneMgr->findObject<ManualObject>(m_btn4->getText());
	m_pObj5 = m_pCurSceneMgr->findObject<ManualObject>(m_btn5->getText());
	m_pObj6 = m_pCurSceneMgr->findObject<ManualObject>(m_btn6->getText());
	m_pObj7 = m_pCurSceneMgr->findObject<ManualObject>(m_btn7->getText());
	m_pObj8 = m_pCurSceneMgr->findObject<ManualObject>(m_btn8->getText());
	m_pObj9 = m_pCurSceneMgr->findObject<ManualObject>(m_btn9->getText());
	m_pObj10 = m_pCurSceneMgr->findObject<ManualObject>(m_btn10->getText());
	m_pObj11 = m_pCurSceneMgr->findObject<ManualObject>(m_btn11->getText());
	m_pObj12 = m_pCurSceneMgr->findObject<ManualObject>(m_btn12->getText());
	m_pObj13 = m_pCurSceneMgr->findObject<ManualObject>(m_btn13->getText());
	m_pObj14 = m_pCurSceneMgr->findObject<ManualObject>(m_btn14->getText());

	m_pCube1 = m_pCurSceneMgr->findObject<Entity>(Root::getSingleton().getLanguage()->getLanguageByID(21));
	m_pCubeBorder1 = m_pCurSceneMgr->findObject<Entity>(Root::getSingleton().getLanguage()->getLanguageByID(22));
	m_pCube2 = m_pCurSceneMgr->findObject<Entity>(Root::getSingleton().getLanguage()->getLanguageByID(23));
	m_pCubeBorder2 = m_pCurSceneMgr->findObject<Entity>(Root::getSingleton().getLanguage()->getLanguageByID(24));

	m_pSphere1 = m_pCurSceneMgr->findObject<ManualObject>(Root::getSingleton().getLanguage()->getLanguageByID(25));
	m_pSphereBorder1 = m_pCurSceneMgr->findObject<ManualObject>(Root::getSingleton().getLanguage()->getLanguageByID(26));
	m_pSphere2 = m_pCurSceneMgr->findObject<ManualObject>(Root::getSingleton().getLanguage()->getLanguageByID(27));
	m_pSphereBorder2 = m_pCurSceneMgr->findObject<ManualObject>(Root::getSingleton().getLanguage()->getLanguageByID(28));

	initAllViewState();
}

ModelPage::~ModelPage()
{
}

void ModelPage::open(bool bBlockMode /*= true*/)
{
	UIDialog::open(bBlockMode);
}

void ModelPage::handleInput()
{
	Camera* pActiveCamera = m_pCurSceneMgr->getActiveCamera();

	if (m_btn1->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj1, true, pActiveCamera);
	}

	if (m_btn2->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj2, true, pActiveCamera);
	}

	if (m_btn3->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj3, true, pActiveCamera);
	}

	if (m_btn4->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj4, true, pActiveCamera);
	}

	if (m_btn5->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj5, true, pActiveCamera);
	}

	if (m_btn6->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj6, true, pActiveCamera);
	}

	if (m_btn7->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj7, true, pActiveCamera);
	}

	if (m_btn8->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj8, true, pActiveCamera);
	}

	if (m_btn9->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj9, true, pActiveCamera);
	}

	if (m_btn10->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj10, true, pActiveCamera);
	}

	if (m_btn11->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj11, true, pActiveCamera);
	}

	if (m_btn12->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj12, true, pActiveCamera);
	}

	if (m_btn13->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj13, true, pActiveCamera);
	}

	if (m_btn14->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pObj14, true, pActiveCamera);
	}

	if (m_btn15->isClicked())
	{
		hideAllModel(pActiveCamera);
		switchShow(m_pCube1, true, pActiveCamera);
		switchShow(m_pCubeBorder1, true, pActiveCamera, false);
		switchShow(m_pCube2, true, pActiveCamera, false);
		switchShow(m_pCubeBorder2, true, pActiveCamera, false);
		switchShow(m_pSphere1, true, pActiveCamera, false);
		switchShow(m_pSphereBorder1, true, pActiveCamera, false);
		switchShow(m_pSphere2, true, pActiveCamera, false);
		switchShow(m_pSphereBorder2, true, pActiveCamera, false);
	}
}

void ModelPage::switchShow(MovableObject* pObj, bool isShow, Camera* camera, bool saveViewMemento /*= true*/)
{
	if (NULL == pObj || NULL == camera || pObj->isVisible() == isShow)
		return;

	pObj->setVisible(isShow);

	if (saveViewMemento)
	{
		if (isShow)
			camera->restoreViewMemento(pObj->getName());
		else
			camera->createViewMemento(pObj->getName());
	}
}

void ModelPage::hideAllModel(Camera* camera)
{
	switchShow(m_pObj1, false, camera);
	switchShow(m_pObj2, false, camera);
	switchShow(m_pObj3, false, camera);
	switchShow(m_pObj4, false, camera);
	switchShow(m_pObj5, false, camera);
	switchShow(m_pObj6, false, camera);
	switchShow(m_pObj7, false, camera);
	switchShow(m_pObj8, false, camera);
	switchShow(m_pObj9, false, camera);
	switchShow(m_pObj10, false, camera);
	switchShow(m_pObj11, false, camera);
	switchShow(m_pObj12, false, camera);
	switchShow(m_pObj13, false, camera);
	switchShow(m_pObj14, false, camera);
	switchShow(m_pCube1, false, camera);
	switchShow(m_pCubeBorder1, false, camera, false);
	switchShow(m_pCube2, false, camera, false);
	switchShow(m_pCubeBorder2, false, camera, false);
	switchShow(m_pSphere1, false, camera, false);
	switchShow(m_pSphereBorder1, false, camera, false);
	switchShow(m_pSphere2, false, camera, false);
	switchShow(m_pSphereBorder2, false, camera, false);
}

void ModelPage::initAllViewState()
{
	Camera* camera = Root::getSingleton().getCurSceneMgr()->getActiveCamera();
	camera->lookAt(Vector3df(0, 0, 150));
	switchShow(m_pObj1, false, camera);
	switchShow(m_pObj2, false, camera);
	switchShow(m_pObj3, false, camera);
	camera->lookAt(Vector3df(20, 0, 0));
	switchShow(m_pObj4, false, camera);
	switchShow(m_pObj5, false, camera);
	camera->lookAt(Vector3df(0, 0, 100));
	switchShow(m_pObj6, false, camera);
	switchShow(m_pObj7, false, camera);
	switchShow(m_pObj8, false, camera);
	switchShow(m_pObj9, false, camera);
	switchShow(m_pObj10, false, camera);
	switchShow(m_pObj11, false, camera);
	switchShow(m_pObj12, false, camera);
	switchShow(m_pObj13, false, camera);
	switchShow(m_pObj14, false, camera);
	switchShow(m_pCube1, false, camera);
	switchShow(m_pCubeBorder1, false, camera, false);
	switchShow(m_pCube2, false, camera, false);
	switchShow(m_pCubeBorder2, false, camera, false);
	switchShow(m_pSphere1, false, camera, false);
	switchShow(m_pSphereBorder1, false, camera, false);
	switchShow(m_pSphere2, false, camera, false);
	switchShow(m_pSphereBorder2, false, camera, false);
}
