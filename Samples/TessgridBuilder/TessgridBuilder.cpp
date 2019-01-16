// TessgridBuilder.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "TessgridBuilder.h"
#include "TessgridBuilderDlg.h"
#include "DlgManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTessgridBuilderApp

BEGIN_MESSAGE_MAP(CTessgridBuilderApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTessgridBuilderApp ����

CTessgridBuilderApp::CTessgridBuilderApp()
	: m_pShellManager(NULL)
	, m_pMainDlg(NULL)
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTessgridBuilderApp ����

CTessgridBuilderApp theApp;


// CTessgridBuilderApp ��ʼ��

BOOL CTessgridBuilderApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	m_pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	m_pMainWnd = m_pMainDlg = CDlgManager::GetIns()->getTessgridBuilderDlg();
	return TRUE;
}

int CTessgridBuilderApp::ExitInstance()
{
	CDlgManager::Destroy();
	// ɾ�����洴���� shell ��������
	CMFCVisualManagerWindows::DestroyInstance();
	if (m_pShellManager)
	{
		delete m_pShellManager;
		m_pShellManager = NULL;
	}

	::CoUninitialize();

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif
	return 0;
}

BOOL CTessgridBuilderApp::OnIdle(LONG lCount)
{
	if (Root::getSingletonPtr() && Root::getSingletonPtr()->IsLaunched())
	{
		Root::getSingleton().renderOneFrame();
		if (m_pMainDlg)
			m_pMainDlg->DisplayModel();
	}

	return CWinApp::OnIdle(lCount);
}
