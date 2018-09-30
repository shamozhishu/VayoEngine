#ifndef __PAGES_H__
#define __PAGES_H__

#include "MyApp.h"

class TestPage : public UIDialog
{
public:
	TestPage();
	~TestPage();
	void open(bool bBlockMode=true);
};

class AnotherPage : public UIDialog
{
public:
	AnotherPage();
	~AnotherPage();
	void open(bool bBlockMode=true);
	void setAttribText(const wstring& attribText);

private:
	UIControl*   m_pCtrl;
	UICheckBox*  m_chec;
	UIStatic*    m_sta;
	UIScrollBar* m_scro;
	UIListBox*   m_listBox;
};

class ModelPage : public UIDialog
{
public:
	ModelPage();
	~ModelPage();
	void open(bool bBlockMode = true);
	void handleInput();

private:
	void switchShow(MovableObject* pObj, bool isShow, Camera* camera, bool saveViewMemento = true);
	void hideAllModel(Camera* camera);
	void initAllViewState();

private:
	UIButton* m_btn1;
	UIButton* m_btn2;
	UIButton* m_btn3;
	UIButton* m_btn4;
	UIButton* m_btn5;
	UIButton* m_btn6;
	UIButton* m_btn7;
	UIButton* m_btn8;
	UIButton* m_btn9;
	UIButton* m_btn10;
	UIButton* m_btn11;
	UIButton* m_btn12;
	UIButton* m_btn13;
	UIButton* m_btn14;
	UIButton* m_btn15;

	ManualObject* m_pObj1;
	ManualObject* m_pObj2;
	ManualObject* m_pObj3;
	ManualObject* m_pObj4;
	ManualObject* m_pObj5;
	ManualObject* m_pObj6;
	ManualObject* m_pObj7;
	ManualObject* m_pObj8;
	ManualObject* m_pObj9;
	ManualObject* m_pObj10;
	ManualObject* m_pObj11;
	ManualObject* m_pObj12;
	ManualObject* m_pObj13;
	ManualObject* m_pObj14;
	MovableObject* m_pCube1;
	MovableObject* m_pCubeBorder1;
	MovableObject* m_pCube2;
	MovableObject* m_pCubeBorder2;
	MovableObject* m_pSphere1;
	MovableObject* m_pSphereBorder1;
	MovableObject* m_pSphere2;
	MovableObject* m_pSphereBorder2;

	SceneManager* m_pCurSceneMgr;
};

class UIInterface : public Singleton<UIInterface>
{
public:
	UIInterface();
	~UIInterface();
	bool init();

private:
	PROPERTY_R(TestPage*,    m_pTestPage,    TestPage)
	PROPERTY_R(AnotherPage*, m_pAnotherPage, AnotherPage)
	PROPERTY_R(ModelPage*,   m_pModelPage,   ModelPage)
};

#endif // __PAGES_H__
