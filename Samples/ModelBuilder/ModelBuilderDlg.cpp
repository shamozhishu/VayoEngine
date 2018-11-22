
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
	DDX_Control(pDX, IDC_BG, m_groupStatic);
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
	m_gridCtrlItem.cxy = 120;
	m_gridCtrlItem.mask = HDI_WIDTH;
	m_wndPropList.GetHeaderCtrl().SetItem(0, &m_gridCtrlItem);
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	COleVariant oleVarStr, oleVarFlt, oleVarUint, oleVarBool;
	oleVarStr.SetString(_T(""), VT_BSTR);
	oleVarFlt.ChangeType(VT_R4);
	oleVarUint.ChangeType(VT_UINT);
	oleVarBool.ChangeType(VT_BOOL);

	CMFCPropertyGridProperty* pGroupProp = new CMFCPropertyGridProperty(_T("属性"));
	m_wndPropList.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("模型名称"), oleVarStr, _T("当前所选择的ManualObject对象名称")));
	CMFCPropertyGridProperty* pValueProp = new CMFCPropertyGridProperty(_T("材质名称"), oleVarStr, _T("所选ManualObject对象的公共材质"));
	pGroupProp->AddSubItem(pValueProp);

	pGroupProp = new CMFCPropertyGridProperty(_T("位置"));
	m_wndPropList.AddProperty(pGroupProp);
	oleVarStr.SetString(_T("XY平面"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("基准平面"), oleVarStr, _T("模型创建所基于的二维平面"));
	pGroupProp->AddSubItem(pValueProp);
	pValueProp->AddOption(_T("XY平面"));
	pValueProp->AddOption(_T("XZ平面"));
	pValueProp->AddOption(_T("YZ平面"));
	pValueProp->AllowEdit(FALSE);
	oleVarFlt.fltVal = 0.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X轴平移"), oleVarFlt, _T("模型在X轴的平移量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y轴平移"), oleVarFlt, _T("模型在Y轴的平移量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z轴平移"), oleVarFlt, _T("模型在Z轴的平移量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X轴旋转"), oleVarFlt, _T("模型在X轴的旋转量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y轴旋转"), oleVarFlt, _T("模型在Y轴的旋转量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z轴旋转"), oleVarFlt, _T("模型在Z轴的旋转量")));
	oleVarFlt.fltVal = 1.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X轴缩放"), oleVarFlt, _T("模型在X轴的缩放量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y轴缩放"), oleVarFlt, _T("模型在Y轴的缩放量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z轴缩放"), oleVarFlt, _T("模型在Z轴的缩放量")));

	pGroupProp = new CMFCPropertyGridProperty(_T("形状"));
	m_wndPropList.AddProperty(pGroupProp);
	CMFCPropertyGridProperty* pChildGroupProp = new CMFCPropertyGridProperty(_T("圆形"));
	pGroupProp->AddSubItem(pChildGroupProp);
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("直径"), oleVarFlt, _T("圆的直径")));
	oleVarFlt.fltVal = 0.0f;
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("圆心X"), oleVarFlt, _T("圆心坐标X")));
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("圆心Y"), oleVarFlt, _T("圆心坐标Y")));
	oleVarUint.uintVal = 60u;
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("采样点"), oleVarUint, _T("组成圆的顶点数量")));
	oleVarBool.boolVal = FALSE;
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("顺时针"), oleVarBool, _T("圆的顶点环绕方式")));
	pChildGroupProp = new CMFCPropertyGridProperty(_T("多边形"));
	pGroupProp->AddSubItem(pChildGroupProp);
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("点坐标X"), oleVarFlt, _T("组成多边形的单个顶点的X坐标")));
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("点坐标Y"), oleVarFlt, _T("组成多边形的单个顶点的Y坐标")));
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("角度"), oleVarFlt, _T("多边形上相邻两个顶点之间的边的弧度，用角度值度量")));
	oleVarUint.uintVal = 1u;
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("采样点"), oleVarUint, _T("组成一条边的顶点数量")));
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("顺时针"), oleVarBool, _T("多边形的顶点环绕方式")));
}

BOOL CModelBuilderDlg::InitToolBar()
{
	if (!m_toolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP |
		CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		return FALSE;
	}

	if (!m_toolbar.LoadToolBar(IDR_TOOLBAR1))
	{
		return FALSE;
	}

	CRect rc;
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rc);
	m_toolbar.MoveWindow(0, rc.top, rc.Width(), 40);

	// 创建位图
	BOOL isSucc = m_img.Create(32, 32, ILC_COLOR32 | ILC_MASK, 2, 2); // 加载图片大小,图片格式,图片数量
	if (isSucc)
	{
		COLORREF co = GetSysColor(COLOR_WINDOW);
		m_img.SetBkColor(co);

		HICON hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_OPEN), IMAGE_ICON, 32, 32, 0);
		m_img.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_SAVE), IMAGE_ICON, 32, 32, 0);
		m_img.Add(hIcon);

		//添加按钮信息
		m_toolbar.GetToolBarCtrl().SetImageList(&m_img);
		m_toolbar.SetButtons(NULL, 4);

		m_toolbar.SetButtonInfo(0, IDC_TOOLBAR_OPEN, TBSTYLE_BUTTON, 0);
		m_toolbar.SetButtonText(0, _T(""));

		m_toolbar.SetButtonInfo(1, -1, TBSTYLE_SEP, -1);
		m_toolbar.SetButtonText(1, _T(""));

		m_toolbar.SetButtonInfo(2, IDC_TOOLBAR_SAVE, TBSTYLE_BUTTON, 1);
		m_toolbar.SetButtonText(2, _T(""));

		m_toolbar.SetButtonInfo(3, -1, TBSTYLE_SEP, -1);
		m_toolbar.SetButtonText(3, _T(""));

		CRect rectToolBar;
		m_toolbar.GetItemRect(0, &rectToolBar);
		m_toolbar.SetSizes(CSize(39, 39), CSize(32, 32));
		m_toolbar.ShowWindow(SW_SHOW);
		m_toolbar.EnableToolTips(TRUE);
	}

	return isSucc;
}

void CModelBuilderDlg::InitStatusBar()
{
	// 添加状态栏
	UINT array[2] = {12301, 12302};
	m_statusbar.Create(this);
	m_statusbar.SetIndicators(array, 2);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	//显示状态栏
	m_statusbar.SetPaneInfo(0, array[0], SBPS_STRETCH, 0);
	m_statusbar.SetPaneInfo(1, array[1], SBPS_POPOUT, 120);
	m_statusbar.SetPaneText(0, _T("就绪"));
	m_statusbar.SetPaneText(1, _T("状态栏"));
}

void CModelBuilderDlg::Resize()
{
	if (!m_modelView)
	{
		return;
	}

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	m_statusbar.SetPaneInfo(0, 12301, SBPS_STRETCH, 0);
	m_statusbar.SetPaneInfo(1, 12302, SBPS_POPOUT, 120);

	CRect rcClientOld;
	GetClientRect(rcClientOld);
	CRect rc = CRect(rcClientOld.Width() - 260, 40, rcClientOld.Width(), rcClientOld.Height() - 19);
	m_wndPropList.MoveWindow(rc);

	rc.left = 0;
	rc.right = rcClientOld.Width() - 260;
	rc.top = 40;
	rc.bottom = rcClientOld.Height() - 19;
	rc.TopLeft() += CPoint(2, 2);
	rc.BottomRight() -= CPoint(0, 2);
	m_modelView->MoveWindow(rc);
}

BEGIN_MESSAGE_MAP(CModelBuilderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, &OnToolTipText)
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

	ShowWindow(SW_SHOWMAXIMIZED);
	m_groupStatic.ShowWindow(SW_HIDE);
	m_mainMenu.LoadMenu(IDR_MENU1);
	SetMenu(&m_mainMenu);
	InitToolBar();
	InitStatusBar();
	// TODO: 在此添加额外的初始化代码
	m_modelView = new CModelView();
	m_modelView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 200, 200), this, 999);
	if (!m_modelView->Init())
		return FALSE;

	InitPropGridCtrl();
	Resize();
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

void CModelBuilderDlg::OnSize(UINT nType, int cx, int cy)
{
	Resize();
}

BOOL CModelBuilderDlg::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	if (pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW)
	{
		TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
		TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
		CString strTipText;
		nID = pNMHDR->idFrom;
		if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags&TTF_IDISHWND) || pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags&TTF_IDISHWND))
		{
			nID = ::GetDlgCtrlID((HWND)nID);
		}

		if (nID != 0)
		{
			strTipText.LoadString(nID);
			strTipText = strTipText.Mid(strTipText.Find('/n', 0) + 1);
#ifndef   _UNICODE
			if (pNMHDR->code == TTN_NEEDTEXTA)
			{
				lstrcpyn(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
			}
			else
			{
				_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
			}
#else
			if (pNMHDR->code == TTN_NEEDTEXTA)
			{
				_wcstombsz(pTTTA->szText, strTipText, sizeof(pTTTA->szText));
			}
			else
			{
				lstrcpyn(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
			}
#endif
			*pResult = 0;
			::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);
			return TRUE;
		}
	}

	return TRUE;
}
