
// TessgridBuilder.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTessgridBuilderApp:
// �йش����ʵ�֣������ TessgridBuilder.cpp
//
class CTessgridBuilderDlg;
class CTessgridBuilderApp : public CWinApp
{
public:
	CTessgridBuilderApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);

// ʵ��
	DECLARE_MESSAGE_MAP()
private:
	CShellManager* m_pShellManager;
	CTessgridBuilderDlg* m_pMainDlg;
};

extern CTessgridBuilderApp theApp;
