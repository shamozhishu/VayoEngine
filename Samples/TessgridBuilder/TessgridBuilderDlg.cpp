
// TessgridBuilderDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TessgridBuilder.h"
#include "TessgridBuilderDlg.h"
#include "afxdialogex.h"
#include "TessgridView.h"
#include "WzdSplash.h"
#include "GridDataManager.h"

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


// CTessgridBuilderDlg 对话框



CTessgridBuilderDlg::CTessgridBuilderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MODELBUILDER_DIALOG, pParent)
	, m_modelView(NULL)
	, m_treeCircleItem(NULL)
	, m_treePolyItem(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
	}
}

bool CTessgridBuilderDlg::isInitOK() const
{
	if (m_modelView)
	{
		return m_modelView->IsOK();
	}
	return false;
}

void CTessgridBuilderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_wndPropList);
	DDX_Control(pDX, IDC_LIST_CONTOUR_IDX, m_listCtrl);
	DDX_Control(pDX, IDC_TREE_CONTOUR_IDX, m_treeCtrl);
}

BOOL CTessgridBuilderDlg::PreTranslateMessage(MSG* pMsg)
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

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CTessgridBuilderDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
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

void CTessgridBuilderDlg::InitPropGridCtrl()
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

	pGroupProp = new CMFCPropertyGridProperty(_T("盖子"));
	m_wndPropList.AddProperty(pGroupProp);
	pChildGroupProp = new CMFCPropertyGridProperty(_T("盖顶"));
	pGroupProp->AddSubItem(pChildGroupProp);
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标X"), oleVarFlt, _T("顶部平面网格的法线X坐标")));
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标Y"), oleVarFlt, _T("顶部平面网格的法线Y坐标")));
	oleVarFlt.fltVal = 1.0f;
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标Z"), oleVarFlt, _T("顶部平面网格的法线Z坐标")));
	oleVarStr.SetString(_T(""), VT_BSTR);
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("盖顶材质"), oleVarStr, _T("顶部平面网格的材质")));
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("顺时针"), oleVarBool, _T("组成顶部平面网格的轮廓顶点环绕方式")));
	pChildGroupProp = new CMFCPropertyGridProperty(_T("盖底"));
	pGroupProp->AddSubItem(pChildGroupProp);
	oleVarFlt.fltVal = 0.0f;
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标X"), oleVarFlt, _T("底部平面网格的法线X坐标")));
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标Y"), oleVarFlt, _T("底部平面网格的法线Y坐标")));
	oleVarFlt.fltVal = -1.0f;
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标Z"), oleVarFlt, _T("底部平面网格的法线Z坐标")));
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("盖底材质"), oleVarStr, _T("底部平面网格的材质")));
	pChildGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("顺时针"), oleVarBool, _T("组成底部平面网格的轮廓顶点环绕方式")));

	pGroupProp = new CMFCPropertyGridProperty(_T("形体拉伸"));
	m_wndPropList.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("拉伸侧面材质"), oleVarStr, _T("拉伸形体侧面网格的材质")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("顺时针"), oleVarBool, _T("组成拉伸形体侧面网格的轮廓顶点环绕方式")));
	oleVarFlt.fltVal = 0.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X轴平移"), oleVarFlt, _T("相应轮廓几何体在X轴的平移量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y轴平移"), oleVarFlt, _T("相应轮廓几何体在Y轴的平移量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z轴平移"), oleVarFlt, _T("相应轮廓几何体在Z轴的平移量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X轴旋转"), oleVarFlt, _T("相应轮廓几何体在X轴的旋转量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y轴旋转"), oleVarFlt, _T("相应轮廓几何体在Y轴的旋转量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z轴旋转"), oleVarFlt, _T("相应轮廓几何体在Z轴的旋转量")));
	oleVarFlt.fltVal = 1.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X轴缩放"), oleVarFlt, _T("相应轮廓几何体在X轴的缩放量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y轴缩放"), oleVarFlt, _T("相应轮廓几何体在Y轴的缩放量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z轴缩放"), oleVarFlt, _T("相应轮廓几何体在Z轴的缩放量")));
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

		//添加按钮信息
		m_toolbar.GetToolBarCtrl().SetImageList(&m_toolbarImg);
		m_toolbar.SetButtons(NULL, 5);

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

BOOL CTessgridBuilderDlg::InitTreeCtrl()
{
	m_treeCtrl.ModifyStyle(TVS_CHECKBOXES, 0);
	m_treeCtrl.ModifyStyle(0, TVS_CHECKBOXES);
	BOOL isSucc = m_treeImg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2); // 加载图片大小,图片格式,图片数量
	if (isSucc)
	{
		COLORREF co = GetSysColor(COLOR_WINDOW);
		m_treeImg.SetBkColor(co);
		m_treeCtrl.SetImageList(&m_treeImg, 0);

		HICON hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_CIRCLE), IMAGE_ICON, 16, 16, 0);
		m_treeImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_POLYGON), IMAGE_ICON, 16, 16, 0);
		m_treeImg.Add(hIcon);

		m_treeCircleItem = m_treeCtrl.InsertItem(TEXT("圆形轮廓"), 0, 0, NULL);
		//m_treeCtrl.SetItemState(m_treeCircleItem, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);
		m_treeCtrl.SetItemData(m_treeCircleItem, ESCT_CIRCLE);
		m_treePolyItem = m_treeCtrl.InsertItem(TEXT("多边形轮廓"), 1, 1, NULL);
		//m_treeCtrl.SetItemState(m_treePolyItem, INDEXTOSTATEIMAGEMASK(0), TVIS_STATEIMAGEMASK);
		m_treeCtrl.SetItemData(m_treePolyItem, ESCT_POLYGON);
	}

	return isSucc;
}

void CTessgridBuilderDlg::Resize()
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

	CRect rc = CRect(0, rcClientOld.Height() - 219, 200, rcClientOld.Height() - 20);
	m_listCtrl.MoveWindow(rc);

	rc = CRect(0, 42, 200, rcClientOld.Height() - 220);
	m_treeCtrl.MoveWindow(rc);

	rc = CRect(rcClientOld.Width() - 260, 40, rcClientOld.Width(), rcClientOld.Height() - 20);
	m_wndPropList.MoveWindow(rc);

	rc.left = 200;
	rc.right = rcClientOld.Width() - 260;
	rc.top = 40;
	rc.bottom = rcClientOld.Height() - 20;
	rc.TopLeft() += CPoint(0, 2);
	m_modelView->MoveWindow(rc);
}

void CTessgridBuilderDlg::CheckToTree(HTREEITEM hItem, BOOL bCheck)
{
	if (hItem != NULL)
	{
		SetChildCheck(hItem, bCheck);
		SetParentCheck(hItem, bCheck);
	}
}

void CTessgridBuilderDlg::SetChildCheck(HTREEITEM hItem, BOOL bCheck)
{
	HTREEITEM hChildItem = m_treeCtrl.GetChildItem(hItem);
	while (hChildItem)
	{
		m_treeCtrl.SetCheck(hChildItem, bCheck);
		SetChildCheck(hChildItem, bCheck);
		hChildItem = m_treeCtrl.GetNextSiblingItem(hChildItem);
	}
}

void CTessgridBuilderDlg::SetParentCheck(HTREEITEM hItem, BOOL bCheck)
{
	HTREEITEM hParent = m_treeCtrl.GetParentItem(hItem);
	if (hParent == NULL)
		return;

	if (bCheck)
	{
		HTREEITEM hSlibing = m_treeCtrl.GetNextSiblingItem(hItem);
		BOOL bFlag = TRUE;

		// 当前Item的前后兄弟节点中是否全都选中
		while (hSlibing)
		{
			if (!m_treeCtrl.GetCheck(hSlibing))
			{
				bFlag = FALSE; // 后继兄弟节点中有一个没有选中
				break;
			}

			hSlibing = m_treeCtrl.GetNextSiblingItem(hSlibing);
		}

		if (bFlag)
		{
			hSlibing = m_treeCtrl.GetPrevSiblingItem(hItem);

			while (hSlibing)
			{
				if (!m_treeCtrl.GetCheck(hSlibing))
				{
					bFlag = FALSE; // 前驱兄弟节点中有一个没有选中
					break;
				}

				hSlibing = m_treeCtrl.GetPrevSiblingItem(hSlibing);
			}
		}

		// bFlag为TRUE，表示当前节点的所有前后兄弟节点都已选中，因此设置其父节点也为选中
		if (bFlag)
			m_treeCtrl.SetCheck(hParent, TRUE);
	}
	else
	{
		// 当前节点设为未选中，当然其父节点也要设置为未选中
		m_treeCtrl.SetCheck(hParent, FALSE);
	}

	// 递归调用
	SetParentCheck(hParent, m_treeCtrl.GetCheck(hParent));
}

BEGIN_MESSAGE_MAP(CTessgridBuilderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, &OnToolTipText)
	ON_COMMAND(ID_32775, &CTessgridBuilderDlg::OnAboutMe)
	ON_COMMAND(ID_32776, &CTessgridBuilderDlg::OnNewBuild)
	ON_COMMAND(IDC_TOOLBAR_NEW, &CTessgridBuilderDlg::OnNewBuild)
	ON_COMMAND(ID_32771, &CTessgridBuilderDlg::OnOpenFile)
	ON_COMMAND(IDC_TOOLBAR_OPEN, &CTessgridBuilderDlg::OnOpenFile)
	ON_COMMAND(ID_32772, &CTessgridBuilderDlg::OnSaveFile)
	ON_COMMAND(IDC_TOOLBAR_SAVE, &CTessgridBuilderDlg::OnSaveFile)
	ON_COMMAND(ID_32779, &CTessgridBuilderDlg::OnSaveAs)
	ON_COMMAND(IDC_TOOLBAR_SAVEAS, &CTessgridBuilderDlg::OnSaveAs)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_32780, &CTessgridBuilderDlg::OnInsertContour)
	ON_COMMAND(ID_32783, &CTessgridBuilderDlg::OnDeleteContour)
	ON_NOTIFY(NM_TVSTATEIMAGECHANGING, IDC_TREE_CONTOUR_IDX, &CTessgridBuilderDlg::OnNMTVStateImageChangingTreeContourIdx)
END_MESSAGE_MAP()


// CTessgridBuilderDlg 消息处理程序

BOOL CTessgridBuilderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CWzdSplash* wzdSplash = new CWzdSplash();
	wzdSplash->Create(IDB_BITMAP_LOGO, this);
	wzdSplash->CenterWindow();
	wzdSplash->UpdateWindow();

	m_mainMenu.LoadMenu(IDR_MENU1);
	SetMenu(&m_mainMenu);
	InitToolBar();
	InitStatusBar();
	InitTreeCtrl();

	m_modelView = new CTessgridView();
	m_modelView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 200, 200), this, 999);
	if (!m_modelView->Init())
		return FALSE;

	InitPropGridCtrl();
	Resize();

	if (wzdSplash)
	{
		wzdSplash->DestroyWindow();
		delete wzdSplash;
		wzdSplash = NULL;
		ShowWindow(SW_MAXIMIZE);
	}

	UpdateData(FALSE);
	UpdateWindow();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTessgridBuilderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTessgridBuilderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTessgridBuilderDlg::OnSize(UINT nType, int cx, int cy)
{
	Resize();
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
	// TODO: 在此添加命令处理程序代码
}

void CTessgridBuilderDlg::OnOpenFile()
{
	CString defaultPath = Root::getSingleton().getConfigManager()->getSceneConfig().ModelsPath.c_str();
	CString filter = _T("文件 (*.tessgrid)|*.tessgrid||");
	CFileDialog dlg(TRUE, defaultPath, _T(""), OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	if (IDOK == dlg.DoModal())
	{
		if (!CGridDataManager::GetIns().OpenTessgridFile(dlg.GetPathName()))
		{
			AfxMessageBox(_T("打开文件失败！"));
		}
	}
}

void CTessgridBuilderDlg::OnSaveFile()
{
	// TODO: 在此添加命令处理程序代码
}

void CTessgridBuilderDlg::OnSaveAs()
{
	// TODO: 在此添加命令处理程序代码
}

void CTessgridBuilderDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	m_treeCtrl.ScreenToClient(&point);
	UINT uFlags;
	HTREEITEM hItem = m_treeCtrl.HitTest(point, &uFlags);
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		m_treeCtrl.ClientToScreen(&point);
		m_treeCtrl.Select(hItem, TVGN_CARET);
		CMenu menu;
		menu.LoadMenu(IDR_MENU2);
		CMenu* subMenu = menu.GetSubMenu(0);

		if (hItem == m_treeCircleItem || hItem == m_treePolyItem)
		{
			subMenu->RemoveMenu(1, MF_BYPOSITION);
		}

		subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

void CTessgridBuilderDlg::OnInsertContour()
{
	HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
	EShapeContourType contourType = (EShapeContourType)m_treeCtrl.GetItemData(hItem);
	switch (contourType)
	{
	case ESCT_CIRCLE:
		m_treeCtrl.InsertItem(_T("圆形"), 0, 0, m_treeCircleItem);
		m_treeCtrl.Expand(m_treeCircleItem, TVE_EXPAND);
		break;
	case ESCT_POLYGON:
		m_treeCtrl.InsertItem(_T("多边形"), 1, 1, m_treePolyItem);
		m_treeCtrl.Expand(m_treePolyItem, TVE_EXPAND);
		break;
	default:
		break;
	}
}

void CTessgridBuilderDlg::OnDeleteContour()
{
	HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
	if (hItem)
	{
		m_treeCtrl.DeleteItem(hItem);
	}
}

void CTessgridBuilderDlg::OnNMTVStateImageChangingTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	MapWindowPoints((CWnd*)&m_treeCtrl, &pt, 1);
	UINT flag = TVHT_ONITEM;
	HTREEITEM hCurrentItem = m_treeCtrl.HitTest(pt, &flag);
	if (hCurrentItem)
	{
		m_treeCtrl.SelectItem(hCurrentItem);
		if (flag & (TVHT_ONITEMSTATEICON))
		{
			CheckToTree(hCurrentItem, !m_treeCtrl.GetCheck(hCurrentItem));
		}
	}

	*pResult = 0;
}
