
// ModelBuilder.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CModelBuilderApp: 
// �йش����ʵ�֣������ ModelBuilder.cpp
//

class CModelBuilderApp : public CWinApp
{
public:
	CModelBuilderApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);

// ʵ��
	DECLARE_MESSAGE_MAP()
private:
	CShellManager* m_pShellManager;
};

extern CModelBuilderApp theApp;
