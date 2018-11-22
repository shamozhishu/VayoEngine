
// ModelBuilderDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ModelBuilder.h"
#include "ModelBuilderDlg.h"
#include "afxdialogex.h"
#include "ModelView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CModelBuilderDlg 对话框



CModelBuilderDlg::CModelBuilderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MODELBUILDER_DIALOG, pParent)
	, m_modelView(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CModelBuilderDlg* CModelBuilderDlg::StartDlg(CWnd* pParent /*= NULL*/)
{
	CModelBuilderDlg* pDlg = new CModelBuilderDlg();
	pDlg->Create(IDD_MODELBUILDER_DIALOG, pParent);
	return pDlg;
}

void CModelBuilderDlg::FinishDlg(CModelBuilderDlg* &pDlg)
{
	if (pDlg)
	{
		pDlg->m_modelView = NULL;
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
	}
}

bool CModelBuilderDlg::isInitOK() const
{
	if (m_modelView)
	{
		return m_modelView->IsOK();
	}
	return false;
}

void CModelBuilderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_wndPropList);
}

BOOL CModelBuilderDlg::PreTranslateMessage(MSG* pMsg)
{
	if (isInitOK())
	{
		POINT pos;
		Device* pDevice = Root::getSingleton().getActiveDevice();
		switch (pMsg->message)
		{
		case WM_MOUSEWHEEL:
			GetCursorPos(&pos);
			if (WindowFromPoint(pos) == m_modelView)
			{
				pDevice->injectMouseWheel((float)((short)HIWORD(pMsg->wParam)) / (float)WHEEL_DELTA);
				return TRUE;
			}
			break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
			GetCursorPos(&pos);
			if (WindowFromPoint(pos) == m_modelView)
			{
				bool keyDown = (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN);
				pDevice->injectKeyboard(pMsg->wParam, pMsg->lParam, keyDown);

				if (pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP)
					return PreTranslateMessage(pMsg);
				else
					return TRUE;
			}
			break;
		case WM_INPUTLANGCHANGE:
			pDevice->injectInputLanguageChange();
			return 0;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CModelBuilderDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (isInitOK())
	{
		Device* pDevice = Root::getSingleton().getActiveDevice();
		switch (message)
		{
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
				pDevice->injectInactive();
			else
				pDevice->injectActivate();
			return 0;
		case WM_ENTERSIZEMOVE:
			pDevice->injectEnterSizeMove();
			return 0;
		case WM_EXITSIZEMOVE:
			pDevice->injectExitSizeMove();
			return 0;
		}
	}

	return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}

void CModelBuilderDlg::OnCancel()
{
	CDialogEx::OnCancel();
	PostQuitMessage(0);
}

void CModelBuilderDlg::InitPropGridCtrl()
{
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	COleVariant var0;
	var0.ChangeType(VT_I4);

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("属性"));
	var0.SetString(_T("标题1"), VT_BSTR);
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("模型名称"), var0, _T("模型名称")));
	var0.intVal = 0;
	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("材质名称"), var0, _T("材质名称")));
	m_wndPropList.AddProperty(pGroup1);
}

BEGIN_MESSAGE_MAP(CModelBuilderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CModelBuilderDlg 消息处理程序

BOOL CModelBuilderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitPropGridCtrl();
	m_modelView = new CModelView();
	m_modelView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 200, 200), this, 999);
	if (!m_modelView->Init())
		return FALSE;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CModelBuilderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CModelBuilderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CModelBuilderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

