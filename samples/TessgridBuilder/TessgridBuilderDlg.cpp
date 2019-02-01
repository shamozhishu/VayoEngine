#include "stdafx.h"
#include "TessgridBuilder.h"
#include "TessgridBuilderDlg.h"
#include "afxdialogex.h"
#include "TessgridView.h"
#include "WzdSplash.h"
#include "GridDataManager.h"
#include "MsgModelVisitor.h"
#include "MsgMaterialVisitor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class GeneCampare
{
public:
	GeneCampare(SpatialInfo* pCmp) : m_pSpatialInfo(pCmp) {}
	bool operator()(const SpatialInfo& other)
	{
		if (m_pSpatialInfo == &other)
			return true;
		else
			return false;
	}
private:
	SpatialInfo* m_pSpatialInfo;
};

// 用于应用程序“关于”菜单项的CAboutDlg对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV支持

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

// CTessgridBuilderDlg对话框

CTessgridBuilderDlg::CTessgridBuilderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MODELBUILDER_DIALOG, pParent)
	, m_modelView(NULL)
	, m_pMsgVisitor(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTessgridBuilderDlg::~CTessgridBuilderDlg()
{
}

CTessgridBuilderDlg* CTessgridBuilderDlg::StartDlg(CWnd* pParent /*= NULL*/)
{
	CTessgridBuilderDlg* pDlg = new CTessgridBuilderDlg();
	pDlg->Create(IDD_MODELBUILDER_DIALOG, pParent);
	return pDlg;
}

void CTessgridBuilderDlg::FinishDlg(CTessgridBuilderDlg* &pDlg)
{
	if (pDlg)
	{
		pDlg->m_modelView = NULL;
		pDlg->m_modelEditorCtrls.m_listCtrl.DeleteAllItems();
		pDlg->m_modelEditorCtrls.m_treeCtrl.DeleteAllItems();
		pDlg->m_materialEditorCtrls.m_listCtrl.DeleteAllItems();
		SAFE_DELETE(pDlg->m_pMsgVisitor);
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
	}
}

LRESULT CTessgridBuilderDlg::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	if (m_pMsgVisitor)
		m_pMsgVisitor->OnPropertyChanged(wParam, lParam);
	return 0;
}

bool CTessgridBuilderDlg::IsEngineInit() const
{
	if (m_modelView)
		return m_modelView->IsStartup();
	return false;
}

void CTessgridBuilderDlg::DisplayModel()
{
	m_modelEditorCtrls.DisplayModel();
}

BOOL CTessgridBuilderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标,当应用程序主窗口不是对话框时,框架将自动执行此操作
	SetIcon(m_hIcon, TRUE);  // 设置大图标
	SetIcon(m_hIcon, FALSE); // 设置小图标

	CWzdSplash* wzdSplash = new CWzdSplash();
	wzdSplash->Create(IDB_BITMAP_LOGO, this);
	wzdSplash->CenterWindow();
	wzdSplash->UpdateWindow();

	m_mainMenu.LoadMenu(IDR_MENU1);
	SetMenu(&m_mainMenu);

	InitToolBar();
	InitStatusBar();

	if (m_tabCtrl.Create(CMFCTabCtrl::STYLE_3D, CRect(), this, 10000, CMFCTabCtrl::LOCATION_BOTTOM))
	{
		m_modelEditorCtrls.Init(this);
		m_materialEditorCtrls.Init(this);
		m_tabCtrl.AddTab(&(m_modelEditorCtrls.m_treeCtrl), _T("模型编辑器"), 0, FALSE);
		m_tabCtrl.AddTab(&(m_materialEditorCtrls.m_listCtrl), _T("材质编辑器"), 0, FALSE);
	}

	if (!m_pMsgVisitor)
		m_pMsgVisitor = new CMsgModelVisitor(this);

	m_modelView = new CTessgridView();
	m_modelView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 200, 200), this, 9999);
	if (!m_modelView->Init())
		return FALSE;

	ResizeWindow();

	if (wzdSplash)
	{
		wzdSplash->DestroyWindow();
		delete wzdSplash;
		wzdSplash = NULL;
		ShowWindow(SW_MAXIMIZE);
	}

	UpdateData(FALSE);
	UpdateWindow();
	return TRUE;  // 除非将焦点设置到控件,否则返回TRUE
}

void CTessgridBuilderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID_MODEL, m_modelEditorCtrls.m_propGridCtrl);
	DDX_Control(pDX, IDC_LIST_CONTOUR_IDX, m_modelEditorCtrls.m_listCtrl);
	DDX_Control(pDX, IDC_TREE_CONTOUR_IDX, m_modelEditorCtrls.m_treeCtrl);
	DDX_Control(pDX, IDC_STATIC_PROPGRIDBG_MODEL, m_modelEditorCtrls.m_staPropGridBg);
	DDX_Control(pDX, IDC_LIST_MATERIAL, m_materialEditorCtrls.m_listCtrl);
	DDX_Control(pDX, IDC_STATIC_PROPGRIDBG_MATERIAL, m_materialEditorCtrls.m_staPropGridBg);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID_MATERIAL, m_materialEditorCtrls.m_propGridCtrl);
}

BOOL CTessgridBuilderDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) // 屏蔽Esc键
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) // 屏蔽Enter键
		return TRUE;

	if (IsEngineInit())
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
					return CDialog::PreTranslateMessage(pMsg);
				else
					return TRUE;
			}
			break;
		case WM_INPUTLANGCHANGE:
			pDevice->injectInputLanguageChange();
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTessgridBuilderDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (IsEngineInit())
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
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 800;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 600;
			return 0;
		}
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CTessgridBuilderDlg::OnCancel()
{
	CDialog::OnCancel();
	PostQuitMessage(0);
}

BOOL CTessgridBuilderDlg::InitToolBar()
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
	BOOL isSucc = m_toolbarImg.Create(32, 32, ILC_COLOR32 | ILC_MASK, 2, 2); // 加载图片大小,图片格式,图片数量
	if (isSucc)
	{
		COLORREF co = GetSysColor(COLOR_WINDOW);
		m_toolbarImg.SetBkColor(co);

		HICON hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_NEW), IMAGE_ICON, 32, 32, 0);
		m_toolbarImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_OPEN), IMAGE_ICON, 32, 32, 0);
		m_toolbarImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_SAVE), IMAGE_ICON, 32, 32, 0);
		m_toolbarImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_SAVEAS), IMAGE_ICON, 32, 32, 0);
		m_toolbarImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_EXPAND), IMAGE_ICON, 32, 32, 0);
		m_toolbarImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_GENERATEMODEL), IMAGE_ICON, 32, 32, 0);
		m_toolbarImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_BGCOLOR), IMAGE_ICON, 32, 32, 0);
		m_toolbarImg.Add(hIcon);

		//添加按钮信息
		m_toolbar.GetToolBarCtrl().SetImageList(&m_toolbarImg);
		m_toolbar.SetButtons(NULL, 8);

		m_toolbar.SetButtonInfo(0, IDC_TOOLBAR_NEW, TBSTYLE_BUTTON, 0);
		m_toolbar.SetButtonText(0, _T(""));

		m_toolbar.SetButtonInfo(1, IDC_TOOLBAR_OPEN, TBSTYLE_BUTTON, 1);
		m_toolbar.SetButtonText(1, _T(""));

		m_toolbar.SetButtonInfo(2, IDC_TOOLBAR_SAVE, TBSTYLE_BUTTON, 2);
		m_toolbar.SetButtonText(2, _T(""));

		m_toolbar.SetButtonInfo(3, IDC_TOOLBAR_SAVEAS, TBSTYLE_BUTTON, 3);
		m_toolbar.SetButtonText(3, _T(""));

		m_toolbar.SetButtonInfo(4, -1, TBSTYLE_SEP, -1);
		m_toolbar.SetButtonText(4, _T(""));

		m_toolbar.SetButtonInfo(5, IDC_TOOLBAR_EXPAND, TBSTYLE_BUTTON, 4);
		m_toolbar.SetButtonText(5, _T(""));

		m_toolbar.SetButtonInfo(6, IDC_TOOLBAR_RESETCAMERA, TBSTYLE_BUTTON, 5);
		m_toolbar.SetButtonText(6, _T(""));

		m_toolbar.SetButtonInfo(7, IDC_TOOLBAR_BGCOLOR, TBSTYLE_BUTTON, 6);
		m_toolbar.SetButtonText(7, _T(""));

		CRect rectToolBar;
		m_toolbar.GetItemRect(0, &rectToolBar);
		m_toolbar.SetSizes(CSize(39, 39), CSize(32, 32));
		m_toolbar.ShowWindow(SW_SHOW);
		m_toolbar.EnableToolTips(TRUE);
	}

	return isSucc;
}

void CTessgridBuilderDlg::InitStatusBar()
{
	UINT array[2] = {12301, 12302};
	m_statusbar.Create(this);
	m_statusbar.SetIndicators(array, 2);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	m_statusbar.SetPaneInfo(0, array[0], SBPS_STRETCH, 0);
	m_statusbar.SetPaneInfo(1, array[1], SBPS_POPOUT, 120);
	m_statusbar.SetPaneText(0, _T("就绪"));
	m_statusbar.SetPaneText(1, _T("状态栏"));
}

void CTessgridBuilderDlg::ResizeWindow()
{
	if (!m_modelView)
		return;

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	m_statusbar.SetPaneInfo(0, 12301, SBPS_STRETCH, 0);
	m_statusbar.SetPaneInfo(1, 12302, SBPS_POPOUT, 120);

	CRect rc, clientRc, tabctrlRc;
	GetClientRect(clientRc);
	tabctrlRc = rc = CRect(0, 42, 200, clientRc.Height() - 20);
	m_tabCtrl.MoveWindow(tabctrlRc);

	rc.left = rc.Width();
	rc.right = clientRc.Width() - 260;
	rc.top = 42;
	rc.bottom = clientRc.Height() - 20;
	m_modelView->MoveWindow(rc);

	tabctrlRc = CRect(0, 42, 200, clientRc.Height() - 42);
	tabctrlRc.TopLeft() += CPoint(2, 2);
	tabctrlRc.BottomRight() -= CPoint(2, 0);
	CMsgModelVisitor modelVisitor(this);
	modelVisitor.OnSize(clientRc, tabctrlRc);
	CMsgMaterialVisitor materialVisitor(this);
	materialVisitor.OnSize(clientRc, tabctrlRc);
}

BEGIN_MESSAGE_MAP(CTessgridBuilderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, &OnToolTipText)
	ON_COMMAND(ID_32775, &OnAboutMe)
	ON_COMMAND(ID_32776, &OnNewBuild)
	ON_COMMAND(IDC_TOOLBAR_NEW, &OnNewBuild)
	ON_COMMAND(ID_32771, &OnOpenFile)
	ON_COMMAND(IDC_TOOLBAR_OPEN, &OnOpenFile)
	ON_COMMAND(ID_32772, &OnSaveFile)
	ON_COMMAND(IDC_TOOLBAR_SAVE, &OnSaveFile)
	ON_COMMAND(ID_32779, &OnSaveAs)
	ON_COMMAND(IDC_TOOLBAR_SAVEAS, &OnSaveAs)
	ON_COMMAND(IDC_TOOLBAR_RESETCAMERA, &OnResetCamera)
	ON_COMMAND(IDC_TOOLBAR_EXPAND, &OnExpandTreeCtrl)
	ON_COMMAND(IDC_TOOLBAR_BGCOLOR, &OnChangeBgColor)
	ON_COMMAND(ID_32780, &OnInsertContour)
	ON_COMMAND(ID_32783, &OnDeleteContour)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONTOUR_IDX, &OnTvnSelchangedTreeContourIdx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONTOUR_IDX, &OnLvnItemchangedListContourIdx)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_CONTOUR_IDX, &OnTvnDeleteitemTreeContourIdx)
	ON_COMMAND(ID_32787, &OnAddStretchBody)
	ON_COMMAND(ID_32788, &OnRemoveStretchBody)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_CONTOUR_IDX, &OnLvnDeleteitemListContourIdx)
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGE_ACTIVE_TAB, &OnActiveTabChanging)
END_MESSAGE_MAP()

// CTessgridBuilderDlg 消息处理程序

// 如果向对话框添加最小化按钮,则需要下面的代码
// 来绘制该图标. 对于使用文档/视图模型的MFC应用程序
// 这将由框架自动完成.

void CTessgridBuilderDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// 当用户拖动最小化窗口时系统调用此函数取得光标显示
HCURSOR CTessgridBuilderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTessgridBuilderDlg::OnSize(UINT nType, int cx, int cy)
{
	ResizeWindow();
}

void CTessgridBuilderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

void CTessgridBuilderDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (m_pMsgVisitor)
		m_pMsgVisitor->OnContextMenu(pWnd, point);
}

BOOL CTessgridBuilderDlg::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
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

void CTessgridBuilderDlg::OnAboutMe()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CTessgridBuilderDlg::OnNewBuild()
{
	if (m_pMsgVisitor)
		m_pMsgVisitor->OnNewBuild();
}

void CTessgridBuilderDlg::OnOpenFile()
{
	if (m_pMsgVisitor)
		m_pMsgVisitor->OnOpenFile();
}

void CTessgridBuilderDlg::OnSaveFile()
{
	if (m_pMsgVisitor)
		m_pMsgVisitor->OnSaveFile();
}

void CTessgridBuilderDlg::OnSaveAs()
{
	if (m_pMsgVisitor)
		m_pMsgVisitor->OnSaveAs();
}

void CTessgridBuilderDlg::OnResetCamera()
{
	if (m_pMsgVisitor)
		m_pMsgVisitor->OnResetCamera();
}

void CTessgridBuilderDlg::OnExpandTreeCtrl()
{
	if (m_pMsgVisitor)
		m_pMsgVisitor->OnExpandTreeCtrl();
}

void CTessgridBuilderDlg::OnChangeBgColor()
{
	if (m_pMsgVisitor)
		m_pMsgVisitor->OnChangeBgColor();
}

void CTessgridBuilderDlg::OnInsertContour()
{
	HTREEITEM hItem = m_modelEditorCtrls.m_treeCtrl.GetSelectedItem();
	if (NULL == hItem || NULL == m_modelEditorCtrls.m_treeCtrl.GetParentItem(hItem))
		return;

	CString itemText = m_modelEditorCtrls.m_treeCtrl.GetItemText(hItem);
	if (itemText == _T("圆形轮廓"))
	{
		HTREEITEM hChildItem = m_modelEditorCtrls.m_treeCtrl.InsertItem(_T("圆形"), EICON_CIRCLE, EICON_CIRCLE, hItem);
		m_modelEditorCtrls.m_treeCtrl.Expand(hItem, TVE_EXPAND);
		if (m_modelEditorCtrls.m_hParentModelItem)
		{
			CGridData* pGridData = (CGridData*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_hParentModelItem);
			if (pGridData)
			{
				pGridData->m_circles.push_back(CGridCircle());
				pGridData->m_needUpdateModel = true;
				m_modelEditorCtrls.m_treeCtrl.SetItemData(hChildItem, (DWORD_PTR)&(pGridData->m_circles.back()));
			}
		}
	}
	else if (itemText == _T("多边形轮廓"))
	{
		HTREEITEM hChildItem = m_modelEditorCtrls.m_treeCtrl.InsertItem(_T("多边形"), EICON_POLYGON, EICON_POLYGON, hItem);
		m_modelEditorCtrls.m_treeCtrl.Expand(hItem, TVE_EXPAND);
		if (m_modelEditorCtrls.m_hParentModelItem)
		{
			CGridData* pGridData = (CGridData*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_hParentModelItem);
			if (pGridData)
			{
				pGridData->m_polygons.push_back(CGridPolygon());
				pGridData->m_needUpdateModel = true;
				m_modelEditorCtrls.m_treeCtrl.SetItemData(hChildItem, (DWORD_PTR)&(pGridData->m_polygons.back()));
			}
		}
	}
	else if (itemText == _T("多边形"))
	{
		HTREEITEM hChildItem = m_modelEditorCtrls.m_treeCtrl.InsertItem(_T("顶点"), EICON_POLYPOINT, EICON_POLYPOINT, hItem);
		m_modelEditorCtrls.m_treeCtrl.Expand(hItem, TVE_EXPAND);
		CGridPolygon* pGridPoly = (CGridPolygon*)m_modelEditorCtrls.m_treeCtrl.GetItemData(hItem);
		if (pGridPoly)
		{
			pGridPoly->m_polyPts.push_back(CGridPolygon::SPolyPoint());
			if (m_modelEditorCtrls.m_hParentModelItem)
			{
				CGridData* pGridData = (CGridData*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_hParentModelItem);
				if (pGridData)
					pGridData->m_needUpdateModel = true;
			}
			m_modelEditorCtrls.m_treeCtrl.SetItemData(hChildItem, (DWORD_PTR)&(pGridPoly->m_polyPts.back()));
		}
	}
}

void CTessgridBuilderDlg::OnDeleteContour()
{
	HTREEITEM hItem = m_modelEditorCtrls.m_treeCtrl.GetSelectedItem();
	if (hItem)
	{
		m_modelEditorCtrls.m_treeCtrlDeleteGridData = true;
		m_modelEditorCtrls.m_treeCtrl.DeleteItem(hItem);
		m_modelEditorCtrls.m_treeCtrlDeleteGridData = false;
		if (m_modelEditorCtrls.m_treeCtrl.GetCount() == 0)
		{
			m_modelEditorCtrls.m_curSelItemType = EICON_NONE;
		}
	}
}

void CTessgridBuilderDlg::OnAddStretchBody()
{
	switch (m_modelEditorCtrls.m_curSelItemType)
	{
	case EICON_CIRCLE:
		if (m_modelEditorCtrls.m_curSelTreeCtrlItem != NULL)
		{
			CGridCircle* pGridCircle = (CGridCircle*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_curSelTreeCtrlItem);
			if (pGridCircle)
			{
				list<SpatialInfo>& refList = pGridCircle->m_shapeOp.m_stretchingBodies.spaceInfo;
				refList.push_back(SpatialInfo());
				int nItem = m_modelEditorCtrls.m_listCtrl.InsertItem(m_modelEditorCtrls.m_listCtrl.GetItemCount(), _T("拉伸形体"), 0);
				m_modelEditorCtrls.m_listCtrl.SetItemData(nItem, (DWORD_PTR)&refList.back());

				CString strStretchNum;
				strStretchNum.Format(_T("形体拉伸次数：%d次"), m_modelEditorCtrls.m_listCtrl.GetItemCount());
				LVCOLUMN lvcol;
				lvcol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE;
				lvcol.fmt = LVCFMT_CENTER;
				lvcol.iImage = 0;
				lvcol.cchTextMax = 256;
				lvcol.cx = 192;
				lvcol.pszText = (LPWSTR)strStretchNum.GetString();
				m_modelEditorCtrls.m_listCtrl.SetColumn(0, &lvcol);
				m_modelEditorCtrls.NeedRefreshModel();
			}
		}
		break;
	case EICON_POLYGON:
		if (m_modelEditorCtrls.m_curSelTreeCtrlItem != NULL)
		{
			CGridPolygon* pGridPoly = (CGridPolygon*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_curSelTreeCtrlItem);
			if (pGridPoly)
			{
				list<SpatialInfo>& refList = pGridPoly->m_shapeOp.m_stretchingBodies.spaceInfo;
				refList.push_back(SpatialInfo());
				int nItem = m_modelEditorCtrls.m_listCtrl.InsertItem(m_modelEditorCtrls.m_listCtrl.GetItemCount(), _T("拉伸形体"), 0);
				m_modelEditorCtrls.m_listCtrl.SetItemData(nItem, (DWORD_PTR)&refList.back());

				CString strStretchNum;
				strStretchNum.Format(_T("形体拉伸次数：%d次"), m_modelEditorCtrls.m_listCtrl.GetItemCount());
				LVCOLUMN lvcol;
				lvcol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE;
				lvcol.fmt = LVCFMT_CENTER;
				lvcol.iImage = 0;
				lvcol.cchTextMax = 256;
				lvcol.cx = 192;
				lvcol.pszText = (LPWSTR)strStretchNum.GetString();
				m_modelEditorCtrls.m_listCtrl.SetColumn(0, &lvcol);
				m_modelEditorCtrls.NeedRefreshModel();
			}
		}
		break;
	}
}

void CTessgridBuilderDlg::OnRemoveStretchBody()
{
	int mark = m_modelEditorCtrls.m_listCtrl.GetSelectionMark();
	if (mark != -1)
	{
		m_modelEditorCtrls.m_listCtrlDeleteGridData = true;
		m_modelEditorCtrls.m_listCtrl.DeleteItem(mark);
		m_modelEditorCtrls.m_listCtrlDeleteGridData = false;
		CString strStretchNum;
		strStretchNum.Format(_T("形体拉伸次数：%d次"), m_modelEditorCtrls.m_listCtrl.GetItemCount());
		LVCOLUMN lvcol;
		lvcol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.iImage = 0;
		lvcol.cchTextMax = 256;
		lvcol.cx = 192;
		lvcol.pszText = (LPWSTR)strStretchNum.GetString();
		m_modelEditorCtrls.m_listCtrl.SetColumn(0, &lvcol);
	}
}

LRESULT CTessgridBuilderDlg::OnActiveTabChanging(WPARAM wParam, LPARAM lParam)
{
	CMFCTabCtrl* tabCtrl = reinterpret_cast<CMFCTabCtrl*>(lParam);
	CWnd* pWnd = tabCtrl->GetActiveWnd();
	if (pWnd == &m_modelEditorCtrls.m_treeCtrl)
	{
		if (m_pMsgVisitor)
		{
			m_pMsgVisitor->ShowCtrls(false);
			delete m_pMsgVisitor;
			m_pMsgVisitor = NULL;
		}

		m_pMsgVisitor = new CMsgModelVisitor(this);
		m_pMsgVisitor->ShowCtrls(true);
		ResizeWindow();
	}
	else if (pWnd == &m_materialEditorCtrls.m_listCtrl)
	{
		if (m_pMsgVisitor)
		{
			m_pMsgVisitor->ShowCtrls(false);
			delete m_pMsgVisitor;
			m_pMsgVisitor = NULL;
		}

		m_pMsgVisitor = new CMsgMaterialVisitor(this);
		m_pMsgVisitor->ShowCtrls(true);
		ResizeWindow();
	}

	return 0;
}

void CTessgridBuilderDlg::OnTvnSelchangedTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_modelEditorCtrls.m_activeSelchangedAfxMsg)
	{
		bool isDoStretchBody = false;
		m_modelEditorCtrls.m_listCtrl.DeleteAllItems();
		m_modelEditorCtrls.m_curSelItemType = EICON_NONE;
		m_modelEditorCtrls.m_hParentModelItem = NULL;
		m_modelEditorCtrls.m_curSelTreeCtrlItem = NULL;
		m_modelEditorCtrls.m_curSelListCtrlItem = -1;
		LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
		if (pNMTreeView && pNMTreeView->itemOld.hItem != pNMTreeView->itemNew.hItem)
		{
			HTREEITEM hItem = pNMTreeView->itemNew.hItem;
			CString itemText = m_modelEditorCtrls.m_treeCtrl.GetItemText(hItem);
			if (NULL == m_modelEditorCtrls.m_treeCtrl.GetParentItem(hItem))
			{
				m_modelEditorCtrls.m_curSelItemType = EICON_MODEL;
				m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE);
				m_modelEditorCtrls.RefreshPropGridCtrl(EICON_MODEL, hItem);
			}
			else if (itemText == _T("圆形"))
			{
				isDoStretchBody = true;
				m_modelEditorCtrls.m_curSelItemType = EICON_CIRCLE;
				m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_CIRCLE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
				m_modelEditorCtrls.RefreshPropGridCtrl(EICON_CIRCLE, hItem);
			}
			else if (itemText == _T("多边形"))
			{
				isDoStretchBody = true;
				m_modelEditorCtrls.m_curSelItemType = EICON_POLYGON;
				m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
				m_modelEditorCtrls.RefreshPropGridCtrl(EICON_POLYGON, hItem);
			}
			else if (itemText == _T("顶点"))
			{
				m_modelEditorCtrls.m_curSelItemType = EICON_POLYPOINT;
				m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_POLY);
				m_modelEditorCtrls.RefreshPropGridCtrl(EICON_POLYPOINT, hItem);
			}
			else if (itemText == _T("圆形轮廓"))
			{
				m_modelEditorCtrls.m_curSelItemType = EICON_CIRCLECONTOUR;
				m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
				m_modelEditorCtrls.RefreshPropGridCtrl(EICON_CIRCLECONTOUR, hItem);
			}
			else if (itemText == _T("多边形轮廓"))
			{
				m_modelEditorCtrls.m_curSelItemType = EICON_POLYCONTOUR;
				m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
				m_modelEditorCtrls.RefreshPropGridCtrl(EICON_POLYCONTOUR, hItem);
			}

			HTREEITEM hParentItem = hItem;
			while (hItem)
			{
				hParentItem = hItem;
				hItem = m_modelEditorCtrls.m_treeCtrl.GetParentItem(hItem);
			}

			if (hParentItem)
			{
				m_modelEditorCtrls.m_hParentModelItem = hParentItem;
				CGridDataManager::GetIns().GeneratingModel(m_modelEditorCtrls.m_treeCtrl.GetItemText(hParentItem), false);
			}
		}

		CString strStretchNum;
		if (isDoStretchBody)
			strStretchNum.Format(_T("形体拉伸次数：%d次"), m_modelEditorCtrls.m_listCtrl.GetItemCount());
		else
			strStretchNum = _T("无形体拉伸");
		LVCOLUMN lvcol;
		lvcol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.iImage = 0;
		lvcol.cchTextMax = 256;
		lvcol.cx = 192;
		lvcol.pszText = (LPWSTR)strStretchNum.GetString();
		m_modelEditorCtrls.m_listCtrl.SetColumn(0, &lvcol);
	}
	*pResult = 0;
}

void CTessgridBuilderDlg::OnTvnDeleteitemTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (m_modelEditorCtrls.m_treeCtrlDeleteGridData && pNMTreeView && pNMTreeView->itemOld.hItem)
	{
		HTREEITEM hItem = pNMTreeView->itemOld.hItem;
		DWORD_PTR itemData = m_modelEditorCtrls.m_treeCtrl.GetItemData(hItem);
		CString itemText = m_modelEditorCtrls.m_treeCtrl.GetItemText(hItem);
		if (NULL == m_modelEditorCtrls.m_treeCtrl.GetParentItem(hItem))
		{
			CGridData* pGridData = (CGridData*)itemData;
			if (pGridData)
			{
				list<CGridData>& refGridDataset = CGridDataManager::GetIns().GetGridDataset();
				list<CGridData>::iterator it = std::find(refGridDataset.begin(), refGridDataset.end(), *pGridData);
				if (it != refGridDataset.end())
				{
					refGridDataset.erase(it);
					pGridData->m_needUpdateModel = true;
				}
			}

			m_modelEditorCtrls.ShowPropGridCtrl(EPGC_HIDE_ALL);
			CGridDataManager::GetIns().HideOtherModel();
		}
		else if (itemText == _T("圆形"))
		{
			CGridCircle* pGridCircle = (CGridCircle*)itemData;
			HTREEITEM hParentItem = m_modelEditorCtrls.m_treeCtrl.GetParentItem(hItem);
			if (pGridCircle && hParentItem)
			{
				CGridData* pGridData = (CGridData*)m_modelEditorCtrls.m_treeCtrl.GetItemData(hParentItem);
				if (pGridData)
				{
					list<CGridCircle>::iterator it = pGridData->m_circles.begin();
					for (; it != pGridData->m_circles.end(); ++it)
					{
						if (&(*it) == pGridCircle)
						{
							it = pGridData->m_circles.erase(it);
							pGridData->m_needUpdateModel = true;
							break;
						}
					}
				}
			}
		}
		else if (itemText == _T("多边形"))
		{
			CGridPolygon* pGridPoly = (CGridPolygon*)itemData;
			HTREEITEM hParentItem = m_modelEditorCtrls.m_treeCtrl.GetParentItem(hItem);
			if (pGridPoly && hParentItem)
			{
				CGridData* pGridData = (CGridData*)m_modelEditorCtrls.m_treeCtrl.GetItemData(hParentItem);
				if (pGridData)
				{
					list<CGridPolygon>::iterator it = pGridData->m_polygons.begin();
					for (; it != pGridData->m_polygons.end(); ++it)
					{
						if (&(*it) == pGridPoly)
						{
							it = pGridData->m_polygons.erase(it);
							pGridData->m_needUpdateModel = true;
							break;
						}
					}
				}
			}
		}
		else if (itemText == _T("顶点"))
		{
			CGridPolygon::SPolyPoint* pPolyPt = (CGridPolygon::SPolyPoint*)itemData;
			HTREEITEM hParentItem = m_modelEditorCtrls.m_treeCtrl.GetParentItem(hItem);
			if (pPolyPt && hParentItem)
			{
				CGridPolygon* pGridPoly = (CGridPolygon*)m_modelEditorCtrls.m_treeCtrl.GetItemData(hParentItem);
				if (pGridPoly)
				{
					list<CGridPolygon::SPolyPoint>::iterator it = pGridPoly->m_polyPts.begin();
					for (; it != pGridPoly->m_polyPts.end(); ++it)
					{
						if (&(*it) == pPolyPt)
						{
							it = pGridPoly->m_polyPts.erase(it);
							if (m_modelEditorCtrls.m_hParentModelItem)
							{
								CGridData* pGridData = (CGridData*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_hParentModelItem);
								if (pGridData)
									pGridData->m_needUpdateModel = true;
							}
							break;
						}
					}
				}
			}
		}
		else if (itemText == _T("圆形轮廓"))
		{
			CGridData* pGridData = (CGridData*)itemData;
			if (pGridData)
			{
				pGridData->m_circles.clear();
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (itemText == _T("多边形轮廓"))
		{
			CGridData* pGridData = (CGridData*)itemData;
			if (pGridData)
			{
				pGridData->m_polygons.clear();
				pGridData->m_needUpdateModel = true;
			}
		}
	}
	*pResult = 0;
}

void CTessgridBuilderDlg::OnLvnItemchangedListContourIdx(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_modelEditorCtrls.m_activeSelchangedAfxMsg)
	{
		CGridCircle* pGridCircle;
		CGridPolygon* pGridPoly;
		LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
		if (pNMLV && pNMLV->uNewState != 0 && pNMLV->uOldState != pNMLV->uNewState)
		{
			switch (m_modelEditorCtrls.m_curSelItemType)
			{
			case EICON_CIRCLE:
				if (m_modelEditorCtrls.m_curSelListCtrlItem != pNMLV->iItem)
				{
					m_modelEditorCtrls.m_curSelListCtrlItem = pNMLV->iItem;
					m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_CIRCLE | EPGC_STRETCH | EPGC_TOPCAP | EPGC_BOTTOMCAP);
					if (m_modelEditorCtrls.m_curSelTreeCtrlItem != NULL)
					{
						pGridCircle = (CGridCircle*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_curSelTreeCtrlItem);
						if (pGridCircle)
							m_modelEditorCtrls.SetStretchBodyGridCtrlValue(pGridCircle->m_shapeOp.m_stretchingBodies, pNMLV->iItem);
					}
				}
				break;
			case EICON_POLYGON:
				if (m_modelEditorCtrls.m_curSelListCtrlItem != pNMLV->iItem)
				{
					m_modelEditorCtrls.m_curSelListCtrlItem = pNMLV->iItem;
					m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_STRETCH | EPGC_TOPCAP | EPGC_BOTTOMCAP);
					if (m_modelEditorCtrls.m_curSelTreeCtrlItem != NULL)
					{
						pGridPoly = (CGridPolygon*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_curSelTreeCtrlItem);
						if (pGridPoly)
							m_modelEditorCtrls.SetStretchBodyGridCtrlValue(pGridPoly->m_shapeOp.m_stretchingBodies, pNMLV->iItem);
					}
				}
				break;
			default:
				break;
			}
		}
		else if (pNMLV && pNMLV->uNewState == 0)
		{
			switch (m_modelEditorCtrls.m_curSelItemType)
			{
			case EICON_CIRCLE:
				m_modelEditorCtrls.m_curSelListCtrlItem = -1;
				m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_CIRCLE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
				break;
			case EICON_POLYGON:
				m_modelEditorCtrls.m_curSelListCtrlItem = -1;
				m_modelEditorCtrls.ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
				break;
			}
		}
	}
	*pResult = 0;
}

void CTessgridBuilderDlg::OnLvnDeleteitemListContourIdx(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (m_modelEditorCtrls.m_listCtrlDeleteGridData && pNMLV && pNMLV->iItem != -1)
	{
		SpatialInfo* pSpatialInfo = (SpatialInfo*)m_modelEditorCtrls.m_listCtrl.GetItemData(pNMLV->iItem);
		if (pSpatialInfo)
		{
			switch (m_modelEditorCtrls.m_curSelItemType)
			{
			case EICON_CIRCLE:
				if (m_modelEditorCtrls.m_curSelTreeCtrlItem != NULL)
				{
					CGridCircle* pGridCircle = (CGridCircle*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_curSelTreeCtrlItem);
					if (pGridCircle)
					{
						list<SpatialInfo>& refList = pGridCircle->m_shapeOp.m_stretchingBodies.spaceInfo;
						list<SpatialInfo>::const_iterator it = std::find_if(refList.cbegin(), refList.cend(), GeneCampare(pSpatialInfo));
						if (it != refList.cend())
						{
							refList.erase(it);
							m_modelEditorCtrls.NeedRefreshModel();
						}
					}
				}
				break;
			case EICON_POLYGON:
				if (m_modelEditorCtrls.m_curSelTreeCtrlItem != NULL)
				{
					CGridPolygon* pGridPoly = (CGridPolygon*)m_modelEditorCtrls.m_treeCtrl.GetItemData(m_modelEditorCtrls.m_curSelTreeCtrlItem);
					if (pGridPoly)
					{
						list<SpatialInfo>& refList = pGridPoly->m_shapeOp.m_stretchingBodies.spaceInfo;
						list<SpatialInfo>::const_iterator it = std::find_if(refList.cbegin(), refList.cend(), GeneCampare(pSpatialInfo));
						if (it != refList.cend())
						{
							refList.erase(it);
							m_modelEditorCtrls.NeedRefreshModel();
						}
					}
				}
				break;
			}
		}
	}
	*pResult = 0;
}
