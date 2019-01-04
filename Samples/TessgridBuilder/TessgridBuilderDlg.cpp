// TessgridBuilderDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "TessgridBuilder.h"
#include "TessgridBuilderDlg.h"
#include "afxdialogex.h"
#include "TessgridView.h"
#include "WzdSplash.h"
#include "GridDataManager.h"
#include "TessgridApp.h"

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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// CTessgridBuilderDlg �Ի���

CTessgridBuilderDlg::CTessgridBuilderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MODELBUILDER_DIALOG, pParent)
	, m_modelView(NULL)
	, m_isTreeCtrlExpand(false)
	, m_listCtrlDeleteGridData(false)
	, m_curSelItemType(EICON_NONE)
	, m_hParentModelItem(NULL)
	, m_curSelTreeCtrlItem(NULL)
	, m_curSelListCtrlItem(-1)
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
		pDlg->m_listCtrl.DeleteAllItems();
		pDlg->m_treeCtrl.DeleteAllItems();
		pDlg->DestroyWindow();
		delete pDlg;
		pDlg = NULL;
	}
}

bool CTessgridBuilderDlg::IsInitOK() const
{
	if (m_modelView)
	{
		return m_modelView->IsOK();
	}
	return false;
}

void CTessgridBuilderDlg::DisplayModel()
{
	HTREEITEM hParentItem;
	HTREEITEM hItem = hParentItem = m_treeCtrl.GetSelectedItem();
	while (hItem)
	{
		hParentItem = hItem;
		hItem = m_treeCtrl.GetParentItem(hItem);
	}

	if (hParentItem)
		CGridDataManager::GetIns().GeneratingModel(m_treeCtrl.GetItemText(hParentItem), true);
}

void CTessgridBuilderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_wndPropList);
	DDX_Control(pDX, IDC_LIST_CONTOUR_IDX, m_listCtrl);
	DDX_Control(pDX, IDC_TREE_CONTOUR_IDX, m_treeCtrl);
	DDX_Control(pDX, IDC_STATIC_STRETCHNUM, m_staStretchNum);
}

BOOL CTessgridBuilderDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) // ����Esc��
		return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) // ����Enter��
		return TRUE;

	if (IsInitOK())
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
	if (IsInitOK())
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

	CMFCPropertyGridProperty* pGroupProp = new CMFCPropertyGridProperty(_T("����"));
	m_wndPropList.AddProperty(pGroupProp);
	CMFCPropertyGridProperty* pChildProp = new CMFCPropertyGridProperty(_T("ģ������"), oleVarStr, _T("��ǰ��ѡ���ManualObject��������"));
	pGroupProp->AddSubItem(pChildProp);
	pChildProp->AllowEdit(FALSE);
	CMFCPropertyGridProperty* pValueProp = new CMFCPropertyGridProperty(_T("��������"), oleVarStr, _T("��ѡManualObject����Ĺ�������"));
	pGroupProp->AddSubItem(pValueProp);

	pGroupProp = new CMFCPropertyGridProperty(_T("λ��"));
	m_wndPropList.AddProperty(pGroupProp);
	oleVarStr.SetString(_T("XYƽ��"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("��׼ƽ��"), oleVarStr, _T("ģ�ʹ��������ڵĶ�άƽ��"));
	pGroupProp->AddSubItem(pValueProp);
	pValueProp->AddOption(_T("XYƽ��"));
	pValueProp->AddOption(_T("XZƽ��"));
	pValueProp->AddOption(_T("YZƽ��"));
	pValueProp->AllowEdit(FALSE);
	oleVarFlt.fltVal = 0.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X��ƽ��"), oleVarFlt, _T("ģ����X���ƽ����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y��ƽ��"), oleVarFlt, _T("ģ����Y���ƽ����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z��ƽ��"), oleVarFlt, _T("ģ����Z���ƽ����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X����ת"), oleVarFlt, _T("ģ����X�����ת��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y����ת"), oleVarFlt, _T("ģ����Y�����ת��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z����ת"), oleVarFlt, _T("ģ����Z�����ת��")));
	oleVarFlt.fltVal = 1.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X������"), oleVarFlt, _T("ģ����X���������")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y������"), oleVarFlt, _T("ģ����Y���������")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z������"), oleVarFlt, _T("ģ����Z���������")));

	pGroupProp = new CMFCPropertyGridProperty(_T("Բ��"));
	m_wndPropList.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("ֱ��"), oleVarFlt, _T("Բ��ֱ��")));
	oleVarFlt.fltVal = 0.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Բ��X"), oleVarFlt, _T("Բ������X")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Բ��Y"), oleVarFlt, _T("Բ������Y")));
	oleVarUint.uintVal = 60u;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("������"), oleVarUint, _T("���Բ�Ķ�������")));
	oleVarBool.boolVal = FALSE;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("˳ʱ��"), oleVarBool, _T("Բ�Ķ��㻷�Ʒ�ʽ")));
	pGroupProp = new CMFCPropertyGridProperty(_T("����"));
	m_wndPropList.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("������X"), oleVarFlt, _T("��ɶ���εĵ��������X����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("������Y"), oleVarFlt, _T("��ɶ���εĵ��������Y����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("�Ƕ�"), oleVarFlt, _T("�������������������֮��ıߵĻ��ȣ��ýǶ�ֵ����")));
	oleVarUint.uintVal = 1u;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("������"), oleVarUint, _T("���һ���ߵĶ�������")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("˳ʱ��"), oleVarBool, _T("����εĶ��㻷�Ʒ�ʽ")));

	pGroupProp = new CMFCPropertyGridProperty(_T("����"));
	m_wndPropList.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("����������"), oleVarStr, _T("���������������Ĳ���")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("˳ʱ��"), oleVarBool, _T("��������������������������㻷�Ʒ�ʽ")));
	oleVarFlt.fltVal = 0.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X��ƽ��"), oleVarFlt, _T("��Ӧ������������X���ƽ����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y��ƽ��"), oleVarFlt, _T("��Ӧ������������Y���ƽ����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z��ƽ��"), oleVarFlt, _T("��Ӧ������������Z���ƽ����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X����ת"), oleVarFlt, _T("��Ӧ������������X�����ת��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y����ת"), oleVarFlt, _T("��Ӧ������������Y�����ת��")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z����ת"), oleVarFlt, _T("��Ӧ������������Z�����ת��")));
	oleVarFlt.fltVal = 1.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("X������"), oleVarFlt, _T("��Ӧ������������X���������")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Y������"), oleVarFlt, _T("��Ӧ������������Y���������")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("Z������"), oleVarFlt, _T("��Ӧ������������Z���������")));

	pGroupProp = new CMFCPropertyGridProperty(_T("�Ƕ�"));
	m_wndPropList.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("����Ƕ�"), oleVarBool, _T("�Ƿ�ӵ�ж�������")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("��������X"), oleVarFlt, _T("����ƽ������ķ���X����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("��������Y"), oleVarFlt, _T("����ƽ������ķ���Y����")));
	oleVarFlt.fltVal = 1.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("��������Z"), oleVarFlt, _T("����ƽ������ķ���Z����")));
	oleVarStr.SetString(_T(""), VT_BSTR);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("�Ƕ�����"), oleVarStr, _T("����ƽ������Ĳ���")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("˳ʱ��"), oleVarBool, _T("��ɶ���ƽ��������������㻷�Ʒ�ʽ")));
	pGroupProp = new CMFCPropertyGridProperty(_T("�ǵ�"));
	m_wndPropList.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("����ǵ�"), oleVarBool, _T("�Ƿ�ӵ�еײ�����")));
	oleVarFlt.fltVal = 0.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("��������X"), oleVarFlt, _T("�ײ�ƽ������ķ���X����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("��������Y"), oleVarFlt, _T("�ײ�ƽ������ķ���Y����")));
	oleVarFlt.fltVal = -1.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("��������Z"), oleVarFlt, _T("�ײ�ƽ������ķ���Z����")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("�ǵײ���"), oleVarStr, _T("�ײ�ƽ������Ĳ���")));
	oleVarBool.boolVal = TRUE;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("˳ʱ��"), oleVarBool, _T("��ɵײ�ƽ��������������㻷�Ʒ�ʽ")));

	ShowPropGridCtrl(EPGC_HIDE_ALL);
}

void CTessgridBuilderDlg::ShowPropGridCtrl(int gridCtrlShowState)
{
	if (gridCtrlShowState == EPGC_HIDE_ALL)
	{
		int num = m_wndPropList.GetPropertyCount();
		for (int i = 0; i < num; ++i)
			RecursionShowPropGridCtrl(FALSE, m_wndPropList.GetProperty(i));
	}
	else
	{
		BitState showState = gridCtrlShowState;
		RecursionShowPropGridCtrl(showState.checkState(EPGC_PROP), m_wndPropList.GetProperty(0));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_PLACE), m_wndPropList.GetProperty(1));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_CIRCLE), m_wndPropList.GetProperty(2));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_POLY), m_wndPropList.GetProperty(3));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_STRETCH), m_wndPropList.GetProperty(4));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_TOPCAP), m_wndPropList.GetProperty(5));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_BOTTOMCAP), m_wndPropList.GetProperty(6));
	}
}

void CTessgridBuilderDlg::RefreshPropGridCtrl(ETreeCtrlItemType itemType, HTREEITEM hItem)
{
	if (!hItem)
		return;

	const CGridData* pGridData;
	CGridCircle* pGridCircle;
	CGridPolygon* pGridPoly;
	CGridPolygon::SPolyPoint* pPolyPt;
	HTREEITEM parentItem;
	m_curSelTreeCtrlItem = NULL;

	switch (itemType)
	{
	case EICON_CIRCLE:
		pGridCircle = (CGridCircle*)m_treeCtrl.GetItemData(hItem);
		if (!pGridCircle)
			break;

		m_wndPropList.GetProperty(2)->GetSubItem(0)->SetValue(pGridCircle->m_diameter);
		m_wndPropList.GetProperty(2)->GetSubItem(1)->SetValue(pGridCircle->m_pos._x);
		m_wndPropList.GetProperty(2)->GetSubItem(2)->SetValue(pGridCircle->m_pos._y);
		m_wndPropList.GetProperty(2)->GetSubItem(3)->SetValue(_variant_t(pGridCircle->m_cnt));
		m_wndPropList.GetProperty(2)->GetSubItem(4)->SetValue(_variant_t(pGridCircle->m_clockwise));

		m_curSelTreeCtrlItem = hItem;
		
		{
			const list<SpatialInfo>& spatialInfos = pGridCircle->m_shapeOp.m_stretchingBodies.spaceInfo;
			list<SpatialInfo>::const_iterator stretchIt = spatialInfos.cbegin();
			for (int i = 0; stretchIt != spatialInfos.cend(); ++stretchIt, ++i)
			{
				m_listCtrl.InsertItem(i, _T("����"), 0);
				m_listCtrl.SetItemData(i, (DWORD_PTR)&(*stretchIt));
			}
		}

		SetTopAndBottomCapGridCtrlValue(pGridCircle->m_shapeOp);
		parentItem = m_treeCtrl.GetParentItem(hItem);
		if (parentItem)
		{
			pGridData = (const CGridData*)m_treeCtrl.GetItemData(parentItem);
			SetPropAndPlaceGridCtrlValue(pGridData);
		}
		break;
	case EICON_POLYGON:
		pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(hItem);
		if (!pGridPoly)
			break;

		m_curSelTreeCtrlItem = hItem;

		{
			const list<SpatialInfo>& spatialInfos = pGridPoly->m_shapeOp.m_stretchingBodies.spaceInfo;
			list<SpatialInfo>::const_iterator stretchIt = spatialInfos.cbegin();
			for (int i = 0; stretchIt != spatialInfos.cend(); ++stretchIt, ++i)
			{
				m_listCtrl.InsertItem(i, _T("����"), 0);
				m_listCtrl.SetItemData(i, (DWORD_PTR)&(*stretchIt));
			}
		}

		SetTopAndBottomCapGridCtrlValue(pGridPoly->m_shapeOp);
		parentItem = m_treeCtrl.GetParentItem(hItem);
		if (parentItem)
		{
			pGridData = (const CGridData*)m_treeCtrl.GetItemData(parentItem);
			SetPropAndPlaceGridCtrlValue(pGridData);
		}
		break;
	case EICON_POLYPOINT:
		pPolyPt = (CGridPolygon::SPolyPoint*)m_treeCtrl.GetItemData(hItem);
		if (!pPolyPt)
			break;

		m_wndPropList.GetProperty(3)->GetSubItem(0)->SetValue(pPolyPt->m_pos._x);
		m_wndPropList.GetProperty(3)->GetSubItem(1)->SetValue(pPolyPt->m_pos._y);
		m_wndPropList.GetProperty(3)->GetSubItem(2)->SetValue(pPolyPt->m_degree);
		m_wndPropList.GetProperty(3)->GetSubItem(3)->SetValue(_variant_t(pPolyPt->m_cnt));
		m_wndPropList.GetProperty(3)->GetSubItem(4)->SetValue(_variant_t(pPolyPt->m_clockwise));

		parentItem = m_treeCtrl.GetParentItem(hItem);
		if (parentItem)
		{
			parentItem = m_treeCtrl.GetParentItem(parentItem);
			if (parentItem)
			{
				pGridData = (const CGridData*)m_treeCtrl.GetItemData(parentItem);
				SetPropAndPlaceGridCtrlValue(pGridData);
			}
		}
		break;
	case EICON_MODEL:
		SetPropAndPlaceGridCtrlValue((const CGridData*)m_treeCtrl.GetItemData(hItem));
		break;
	case EICON_CIRCLECONTOUR:
	case EICON_POLYCONTOUR:
		pGridData = (const CGridData*)m_treeCtrl.GetItemData(hItem);
		SetPropAndPlaceGridCtrlValue(pGridData);
		SetTopAndBottomCapGridCtrlValue(pGridData);
		break;
	default:
		break;
	}
}

void CTessgridBuilderDlg::NeedRefreshModel()
{
	if (m_hParentModelItem)
	{
		CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
		if (pGridData)
			pGridData->m_needUpdateModel = true;
	}
}

void CTessgridBuilderDlg::ResetCtrlLayout()
{
	m_listCtrl.DeleteAllItems();
	if (m_curSelItemType == EICON_NONE)
	{
		ShowPropGridCtrl(EPGC_HIDE_ALL);
	}
	else
	{
		ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE);
		RefreshPropGridCtrl(EICON_MODEL, NULL);
	}
}

void CTessgridBuilderDlg::RecursionShowPropGridCtrl(BOOL bShow, CMFCPropertyGridProperty* pGridProperty)
{
	if (pGridProperty)
	{
		if (pGridProperty->IsVisible() != bShow)
			pGridProperty->Show(bShow);
		int num = pGridProperty->GetSubItemsCount();
		for (int i = 0; i < num; ++i)
		{
			RecursionShowPropGridCtrl(bShow, pGridProperty->GetSubItem(i));
		}
	}
}

bool CTessgridBuilderDlg::SetPropAndPlaceGridCtrlValue(const CGridData* pGridData)
{
	if (!pGridData)
		return false;

	m_wndPropList.GetProperty(0)->GetSubItem(0)->SetValue(pGridData->m_prop.m_modelName.GetString());
	m_wndPropList.GetProperty(0)->GetSubItem(1)->SetValue(pGridData->m_prop.m_materialName.GetString());

	m_wndPropList.GetProperty(1)->GetSubItem(0)->SetValue(pGridData->m_place.m_plane.GetString());
	m_wndPropList.GetProperty(1)->GetSubItem(1)->SetValue(pGridData->m_place.m_spaceInfo._translation._x);
	m_wndPropList.GetProperty(1)->GetSubItem(2)->SetValue(pGridData->m_place.m_spaceInfo._translation._y);
	m_wndPropList.GetProperty(1)->GetSubItem(3)->SetValue(pGridData->m_place.m_spaceInfo._translation._z);
	m_wndPropList.GetProperty(1)->GetSubItem(4)->SetValue(pGridData->m_place.m_spaceInfo._rotation._x);
	m_wndPropList.GetProperty(1)->GetSubItem(5)->SetValue(pGridData->m_place.m_spaceInfo._rotation._y);
	m_wndPropList.GetProperty(1)->GetSubItem(6)->SetValue(pGridData->m_place.m_spaceInfo._rotation._z);
	m_wndPropList.GetProperty(1)->GetSubItem(7)->SetValue(pGridData->m_place.m_spaceInfo._scale._x);
	m_wndPropList.GetProperty(1)->GetSubItem(8)->SetValue(pGridData->m_place.m_spaceInfo._scale._y);
	m_wndPropList.GetProperty(1)->GetSubItem(9)->SetValue(pGridData->m_place.m_spaceInfo._scale._z);

	return true;
}

bool CTessgridBuilderDlg::SetTopAndBottomCapGridCtrlValue(const CGridData* pGridData)
{
	if (!pGridData)
		return false;

	m_wndPropList.GetProperty(5)->GetSubItem(0)->SetValue(_variant_t(pGridData->m_isTopCapTessAll));
	m_wndPropList.GetProperty(5)->GetSubItem(1)->SetValue(pGridData->m_topCap.normal._x);
	m_wndPropList.GetProperty(5)->GetSubItem(1)->Enable(pGridData->m_isTopCapTessAll);
	m_wndPropList.GetProperty(5)->GetSubItem(2)->SetValue(pGridData->m_topCap.normal._y);
	m_wndPropList.GetProperty(5)->GetSubItem(2)->Enable(pGridData->m_isTopCapTessAll);
	m_wndPropList.GetProperty(5)->GetSubItem(3)->SetValue(pGridData->m_topCap.normal._z);
	m_wndPropList.GetProperty(5)->GetSubItem(3)->Enable(pGridData->m_isTopCapTessAll);
	m_wndPropList.GetProperty(5)->GetSubItem(4)->SetValue(pGridData->m_topCap.materialName.GetString());
	m_wndPropList.GetProperty(5)->GetSubItem(4)->Enable(pGridData->m_isTopCapTessAll);
	m_wndPropList.GetProperty(5)->GetSubItem(5)->SetValue(_variant_t(pGridData->m_topCap.reverse));
	m_wndPropList.GetProperty(5)->GetSubItem(5)->Enable(pGridData->m_isTopCapTessAll);

	m_wndPropList.GetProperty(6)->GetSubItem(0)->SetValue(_variant_t(pGridData->m_isBottomCapTessAll));
	m_wndPropList.GetProperty(6)->GetSubItem(1)->SetValue(pGridData->m_bottomCap.normal._x);
	m_wndPropList.GetProperty(6)->GetSubItem(1)->Enable(pGridData->m_isBottomCapTessAll);
	m_wndPropList.GetProperty(6)->GetSubItem(2)->SetValue(pGridData->m_bottomCap.normal._y);
	m_wndPropList.GetProperty(6)->GetSubItem(2)->Enable(pGridData->m_isBottomCapTessAll);
	m_wndPropList.GetProperty(6)->GetSubItem(3)->SetValue(pGridData->m_bottomCap.normal._z);
	m_wndPropList.GetProperty(6)->GetSubItem(3)->Enable(pGridData->m_isBottomCapTessAll);
	m_wndPropList.GetProperty(6)->GetSubItem(4)->SetValue(pGridData->m_bottomCap.materialName.GetString());
	m_wndPropList.GetProperty(6)->GetSubItem(4)->Enable(pGridData->m_isBottomCapTessAll);
	m_wndPropList.GetProperty(6)->GetSubItem(5)->SetValue(_variant_t(pGridData->m_bottomCap.reverse));
	m_wndPropList.GetProperty(6)->GetSubItem(5)->Enable(pGridData->m_isBottomCapTessAll);

	return true;
}

void CTessgridBuilderDlg::SetTopAndBottomCapGridCtrlValue(const CGridShapeOp& shapeOp)
{
	m_wndPropList.GetProperty(5)->GetSubItem(0)->SetValue(_variant_t(shapeOp.m_hasTopCaps));
	m_wndPropList.GetProperty(5)->GetSubItem(1)->SetValue(shapeOp.m_topCap.normal._x);
	m_wndPropList.GetProperty(5)->GetSubItem(1)->Enable(shapeOp.m_hasTopCaps);
	m_wndPropList.GetProperty(5)->GetSubItem(2)->SetValue(shapeOp.m_topCap.normal._y);
	m_wndPropList.GetProperty(5)->GetSubItem(2)->Enable(shapeOp.m_hasTopCaps);
	m_wndPropList.GetProperty(5)->GetSubItem(3)->SetValue(shapeOp.m_topCap.normal._z);
	m_wndPropList.GetProperty(5)->GetSubItem(3)->Enable(shapeOp.m_hasTopCaps);
	m_wndPropList.GetProperty(5)->GetSubItem(4)->SetValue(shapeOp.m_topCap.materialName.GetString());
	m_wndPropList.GetProperty(5)->GetSubItem(4)->Enable(shapeOp.m_hasTopCaps);
	m_wndPropList.GetProperty(5)->GetSubItem(5)->SetValue(_variant_t(shapeOp.m_topCap.reverse));
	m_wndPropList.GetProperty(5)->GetSubItem(5)->Enable(shapeOp.m_hasTopCaps);

	m_wndPropList.GetProperty(6)->GetSubItem(0)->SetValue(_variant_t(shapeOp.m_hasBottomCaps));
	m_wndPropList.GetProperty(6)->GetSubItem(1)->SetValue(shapeOp.m_bottomCap.normal._x);
	m_wndPropList.GetProperty(6)->GetSubItem(1)->Enable(shapeOp.m_hasBottomCaps);
	m_wndPropList.GetProperty(6)->GetSubItem(2)->SetValue(shapeOp.m_bottomCap.normal._y);
	m_wndPropList.GetProperty(6)->GetSubItem(2)->Enable(shapeOp.m_hasBottomCaps);
	m_wndPropList.GetProperty(6)->GetSubItem(3)->SetValue(shapeOp.m_bottomCap.normal._z);
	m_wndPropList.GetProperty(6)->GetSubItem(3)->Enable(shapeOp.m_hasBottomCaps);
	m_wndPropList.GetProperty(6)->GetSubItem(4)->SetValue(shapeOp.m_bottomCap.materialName.GetString());
	m_wndPropList.GetProperty(6)->GetSubItem(4)->Enable(shapeOp.m_hasBottomCaps);
	m_wndPropList.GetProperty(6)->GetSubItem(5)->SetValue(_variant_t(shapeOp.m_bottomCap.reverse));
	m_wndPropList.GetProperty(6)->GetSubItem(5)->Enable(shapeOp.m_hasBottomCaps);
}

void CTessgridBuilderDlg::SetStretchBodyGridCtrlValue(const CGridShapeOp::SStretchingBody& shapeOpStretchBody, int iItem)
{
	m_wndPropList.GetProperty(4)->GetSubItem(0)->SetValue(shapeOpStretchBody.materialName.GetString());
	m_wndPropList.GetProperty(4)->GetSubItem(1)->SetValue(_variant_t(shapeOpStretchBody.reverse));
	if (iItem != -1)
	{
		SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(iItem);
		if (pSpaceData)
		{
			m_wndPropList.GetProperty(4)->GetSubItem(2)->SetValue(pSpaceData->_translation._x);
			m_wndPropList.GetProperty(4)->GetSubItem(3)->SetValue(pSpaceData->_translation._y);
			m_wndPropList.GetProperty(4)->GetSubItem(4)->SetValue(pSpaceData->_translation._z);
			m_wndPropList.GetProperty(4)->GetSubItem(5)->SetValue(pSpaceData->_rotation._x);
			m_wndPropList.GetProperty(4)->GetSubItem(6)->SetValue(pSpaceData->_rotation._y);
			m_wndPropList.GetProperty(4)->GetSubItem(7)->SetValue(pSpaceData->_rotation._z);
			m_wndPropList.GetProperty(4)->GetSubItem(8)->SetValue(pSpaceData->_scale._x);
			m_wndPropList.GetProperty(4)->GetSubItem(9)->SetValue(pSpaceData->_scale._y);
			m_wndPropList.GetProperty(4)->GetSubItem(10)->SetValue(pSpaceData->_scale._z);
		}
	}
}

void CTessgridBuilderDlg::TopCapPropertyChanged(CString propName, CString propValue)
{
	CGridData* pGridData;
	CGridCircle* pGridCircle;
	CGridPolygon* pGridPoly;
	switch (m_curSelItemType)
	{
	case EICON_CIRCLE:
		if (m_curSelTreeCtrlItem != NULL)
		{
			pGridCircle = (CGridCircle*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
			if (pGridCircle)
			{
				if (propName == _T("����Ƕ�"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridCircle->m_shapeOp.m_hasTopCaps)
					{
						pGridCircle->m_shapeOp.m_hasTopCaps = bVal;
						m_wndPropList.GetProperty(5)->GetSubItem(1)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(2)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(3)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(4)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(5)->Enable(bVal);
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_topCap.normal._x))
					{
						pGridCircle->m_shapeOp.m_topCap.normal._x = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_topCap.normal._y))
					{
						pGridCircle->m_shapeOp.m_topCap.normal._y = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_topCap.normal._z))
					{
						pGridCircle->m_shapeOp.m_topCap.normal._z = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("�Ƕ�����"))
				{
					if (propValue != pGridCircle->m_shapeOp.m_topCap.materialName)
					{
						pGridCircle->m_shapeOp.m_topCap.materialName = propValue;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("˳ʱ��"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridCircle->m_shapeOp.m_topCap.reverse)
					{
						pGridCircle->m_shapeOp.m_topCap.reverse = bVal;
						NeedRefreshModel();
					}
				}
			}
		}
		break;
	case EICON_POLYGON:
		if (m_curSelTreeCtrlItem != NULL)
		{
			pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
			if (pGridPoly)
			{
				if (propName == _T("����Ƕ�"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridPoly->m_shapeOp.m_hasTopCaps)
					{
						pGridPoly->m_shapeOp.m_hasTopCaps = bVal;
						m_wndPropList.GetProperty(5)->GetSubItem(1)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(2)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(3)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(4)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(5)->Enable(bVal);
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_topCap.normal._x))
					{
						pGridPoly->m_shapeOp.m_topCap.normal._x = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_topCap.normal._y))
					{
						pGridPoly->m_shapeOp.m_topCap.normal._y = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_topCap.normal._z))
					{
						pGridPoly->m_shapeOp.m_topCap.normal._z = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("�Ƕ�����"))
				{
					if (propValue != pGridPoly->m_shapeOp.m_topCap.materialName)
					{
						pGridPoly->m_shapeOp.m_topCap.materialName = propValue;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("˳ʱ��"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridPoly->m_shapeOp.m_topCap.reverse)
					{
						pGridPoly->m_shapeOp.m_topCap.reverse = bVal;
						NeedRefreshModel();
					}
				}
			}
		}
		break;
	case EICON_CIRCLECONTOUR:
	case EICON_POLYCONTOUR:
		if (m_hParentModelItem != NULL)
		{
			pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			if (pGridData)
			{
				if (propName == _T("����Ƕ�"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridData->m_isTopCapTessAll)
					{
						pGridData->m_isTopCapTessAll = bVal;
						m_wndPropList.GetProperty(5)->GetSubItem(1)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(2)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(3)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(4)->Enable(bVal);
						m_wndPropList.GetProperty(5)->GetSubItem(5)->Enable(bVal);
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("��������X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_topCap.normal._x))
					{
						pGridData->m_topCap.normal._x = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("��������Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_topCap.normal._y))
					{
						pGridData->m_topCap.normal._y = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("��������Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_topCap.normal._z))
					{
						pGridData->m_topCap.normal._z = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("�Ƕ�����"))
				{
					if (propValue != pGridData->m_topCap.materialName)
					{
						pGridData->m_topCap.materialName = propValue;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("˳ʱ��"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridData->m_topCap.reverse)
					{
						pGridData->m_topCap.reverse = bVal;
						pGridData->m_needUpdateModel = true;
					}
				}
			}
		}
		break;
	}
}

void CTessgridBuilderDlg::BottomCapPropertyChanged(CString propName, CString propValue)
{
	CGridData* pGridData;
	CGridCircle* pGridCircle;
	CGridPolygon* pGridPoly;
	switch (m_curSelItemType)
	{
	case EICON_CIRCLE:
		if (m_curSelTreeCtrlItem != NULL)
		{
			pGridCircle = (CGridCircle*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
			if (pGridCircle)
			{
				if (propName == _T("����ǵ�"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridCircle->m_shapeOp.m_hasBottomCaps)
					{
						pGridCircle->m_shapeOp.m_hasBottomCaps = bVal;
						m_wndPropList.GetProperty(6)->GetSubItem(1)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(2)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(3)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(4)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(5)->Enable(bVal);
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_bottomCap.normal._x))
					{
						pGridCircle->m_shapeOp.m_bottomCap.normal._x = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_bottomCap.normal._y))
					{
						pGridCircle->m_shapeOp.m_bottomCap.normal._y = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_bottomCap.normal._z))
					{
						pGridCircle->m_shapeOp.m_bottomCap.normal._z = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("�ǵײ���"))
				{
					if (propValue != pGridCircle->m_shapeOp.m_bottomCap.materialName)
					{
						pGridCircle->m_shapeOp.m_bottomCap.materialName = propValue;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("˳ʱ��"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridCircle->m_shapeOp.m_bottomCap.reverse)
					{
						pGridCircle->m_shapeOp.m_bottomCap.reverse = bVal;
						NeedRefreshModel();
					}
				}
			}
		}
		break;
	case EICON_POLYGON:
		if (m_curSelTreeCtrlItem != NULL)
		{
			pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
			if (pGridPoly)
			{
				if (propName == _T("����ǵ�"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridPoly->m_shapeOp.m_hasBottomCaps)
					{
						pGridPoly->m_shapeOp.m_hasBottomCaps = bVal;
						m_wndPropList.GetProperty(6)->GetSubItem(1)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(2)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(3)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(4)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(5)->Enable(bVal);
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_bottomCap.normal._x))
					{
						pGridPoly->m_shapeOp.m_bottomCap.normal._x = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_bottomCap.normal._y))
					{
						pGridPoly->m_shapeOp.m_bottomCap.normal._y = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("��������Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_bottomCap.normal._z))
					{
						pGridPoly->m_shapeOp.m_bottomCap.normal._z = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("�ǵײ���"))
				{
					if (propValue != pGridPoly->m_shapeOp.m_bottomCap.materialName)
					{
						pGridPoly->m_shapeOp.m_bottomCap.materialName = propValue;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("˳ʱ��"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridPoly->m_shapeOp.m_bottomCap.reverse)
					{
						pGridPoly->m_shapeOp.m_bottomCap.reverse = bVal;
						NeedRefreshModel();
					}
				}
			}
		}
		break;
	case EICON_CIRCLECONTOUR:
	case EICON_POLYCONTOUR:
		if (m_hParentModelItem != NULL)
		{
			pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			if (pGridData)
			{
				if (propName == _T("����ǵ�"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridData->m_isBottomCapTessAll)
					{
						pGridData->m_isBottomCapTessAll = bVal;
						m_wndPropList.GetProperty(6)->GetSubItem(1)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(2)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(3)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(4)->Enable(bVal);
						m_wndPropList.GetProperty(6)->GetSubItem(5)->Enable(bVal);
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("��������X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_bottomCap.normal._x))
					{
						pGridData->m_bottomCap.normal._x = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("��������Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_bottomCap.normal._y))
					{
						pGridData->m_bottomCap.normal._y = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("��������Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_bottomCap.normal._z))
					{
						pGridData->m_bottomCap.normal._z = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("�ǵײ���"))
				{
					if (propValue != pGridData->m_bottomCap.materialName)
					{
						pGridData->m_bottomCap.materialName = propValue;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("˳ʱ��"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridData->m_bottomCap.reverse)
					{
						pGridData->m_bottomCap.reverse = bVal;
						pGridData->m_needUpdateModel = true;
					}
				}
			}
		}
		break;
	}
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

	// ����λͼ
	BOOL isSucc = m_toolbarImg.Create(32, 32, ILC_COLOR32 | ILC_MASK, 2, 2); // ����ͼƬ��С,ͼƬ��ʽ,ͼƬ����
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

		//��Ӱ�ť��Ϣ
		m_toolbar.GetToolBarCtrl().SetImageList(&m_toolbarImg);
		m_toolbar.SetButtons(NULL, 7);

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
	// ���״̬��
	UINT array[2] = {12301, 12302};
	m_statusbar.Create(this);
	m_statusbar.SetIndicators(array, 2);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
	//��ʾ״̬��
	m_statusbar.SetPaneInfo(0, array[0], SBPS_STRETCH, 0);
	m_statusbar.SetPaneInfo(1, array[1], SBPS_POPOUT, 120);
	m_statusbar.SetPaneText(0, _T("����"));
	m_statusbar.SetPaneText(1, _T("״̬��"));
}

BOOL CTessgridBuilderDlg::InitListCtrl()
{
	BOOL isSucc = m_listCtrlImg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2);
	if (isSucc)
	{
		COLORREF co = GetSysColor(COLOR_WINDOW);
		m_listCtrlImg.SetBkColor(co);
		m_listCtrl.SetImageList(&m_listCtrlImg, 0);

		HICON hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0);
		m_listCtrlImg.Add(hIcon);
	}

	return isSucc;
}

BOOL CTessgridBuilderDlg::InitTreeCtrl()
{
	BOOL isSucc = m_treeImg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2);
	if (isSucc)
	{
		COLORREF co = GetSysColor(COLOR_WINDOW);
		m_treeImg.SetBkColor(co);
		m_treeCtrl.SetImageList(&m_treeImg, 0);

		HICON hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_CIRCLE), IMAGE_ICON, 16, 16, 0);
		m_treeImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_POLYGON), IMAGE_ICON, 16, 16, 0);
		m_treeImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_MODEL), IMAGE_ICON, 16, 16, 0);
		m_treeImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_CIRCLECONTOUR), IMAGE_ICON, 16, 16, 0);
		m_treeImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_POLYCONTOUR), IMAGE_ICON, 16, 16, 0);
		m_treeImg.Add(hIcon);
		hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_POLYPOINT), IMAGE_ICON, 16, 16, 0);
		m_treeImg.Add(hIcon);
	}

	return isSucc;
}

void CTessgridBuilderDlg::RefreshTreeCtrl()
{
	m_treeCtrl.DeleteAllItems();
	const std::list<CGridData>& gridDataset = CGridDataManager::GetIns().GetGridDataset();
	std::list<CGridData>::const_iterator cit = gridDataset.cbegin();
	for (; cit != gridDataset.cend(); ++cit)
	{
		const CGridData* pGridData = &(*cit);
		if (!pGridData)
			continue;

		HTREEITEM rootTreeItem = m_treeCtrl.InsertItem(pGridData->m_prop.m_modelName, EICON_MODEL, EICON_MODEL, NULL);
		m_treeCtrl.SetItemData(rootTreeItem, (DWORD_PTR)pGridData);
		HTREEITEM treeItem = m_treeCtrl.InsertItem(_T("Բ������"), EICON_CIRCLECONTOUR, EICON_CIRCLECONTOUR, rootTreeItem);
		m_treeCtrl.SetItemData(treeItem, (DWORD_PTR)pGridData);

		list<CGridCircle>::const_iterator cirIt = pGridData->m_circles.cbegin();
		for (; cirIt != pGridData->m_circles.cend(); ++cirIt)
		{
			m_treeCtrl.SetItemData(m_treeCtrl.InsertItem(_T("Բ��"), EICON_CIRCLE, EICON_CIRCLE, treeItem), (DWORD_PTR)&(*cirIt));
		}

		treeItem = m_treeCtrl.InsertItem(_T("���������"), EICON_POLYCONTOUR, EICON_POLYCONTOUR, rootTreeItem);
		m_treeCtrl.SetItemData(treeItem, (DWORD_PTR)pGridData);
		list<CGridPolygon>::const_iterator polyIt = pGridData->m_polygons.cbegin();
		for (; polyIt != pGridData->m_polygons.cend(); ++polyIt)
		{
			HTREEITEM childItem = m_treeCtrl.InsertItem(_T("�����"), EICON_POLYGON, EICON_POLYGON, treeItem);
			m_treeCtrl.SetItemData(childItem, (DWORD_PTR)&(*polyIt));
			const list<CGridPolygon::SPolyPoint>& polyPts = (*polyIt).m_polyPts;
			list<CGridPolygon::SPolyPoint>::const_iterator ptIt = polyPts.cbegin();
			for (; ptIt != polyPts.cend(); ++ptIt)
			{
				HTREEITEM ptItem = m_treeCtrl.InsertItem(_T("����"), EICON_POLYPOINT, EICON_POLYPOINT, childItem);
				m_treeCtrl.SetItemData(ptItem, (DWORD_PTR)&(*ptIt));
			}
		}
	}
}

void CTessgridBuilderDlg::ExpandTreeCtrl(HTREEITEM treeItem, bool expanding)
{
	m_isTreeCtrlExpand = expanding;
	while (treeItem)
	{
		UINT nCode = expanding ? TVE_EXPAND : TVE_COLLAPSE;
		m_treeCtrl.Expand(treeItem, nCode);
		ExpandTreeCtrl(m_treeCtrl.GetChildItem(treeItem), expanding);
		treeItem = m_treeCtrl.GetNextSiblingItem(treeItem);
	}
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

	CRect rc = CRect(0, rcClientOld.Height() - 80, 200, rcClientOld.Height() - 20);
	m_listCtrl.MoveWindow(rc);

	m_staStretchNum.MoveWindow(0, rcClientOld.Height() - 100, 200, 20);

	rc = CRect(0, 42, 200, rcClientOld.Height() - 101);
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

BEGIN_MESSAGE_MAP(CTessgridBuilderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
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
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_32780, &OnInsertContour)
	ON_COMMAND(ID_32783, &OnDeleteContour)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONTOUR_IDX, &OnTvnSelchangedTreeContourIdx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CONTOUR_IDX, &OnLvnItemchangedListContourIdx)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &OnPropertyChanged)
	ON_NOTIFY(TVN_DELETEITEM, IDC_TREE_CONTOUR_IDX, &OnTvnDeleteitemTreeContourIdx)
	ON_COMMAND(ID_32787, &OnAddStretchBody)
	ON_COMMAND(ID_32788, &OnRemoveStretchBody)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_CONTOUR_IDX, &OnLvnDeleteitemListContourIdx)
END_MESSAGE_MAP()

// CTessgridBuilderDlg ��Ϣ�������

BOOL CTessgridBuilderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	CWzdSplash* wzdSplash = new CWzdSplash();
	wzdSplash->Create(IDB_BITMAP_LOGO, this);
	wzdSplash->CenterWindow();
	wzdSplash->UpdateWindow();

	m_mainMenu.LoadMenu(IDR_MENU1);
	SetMenu(&m_mainMenu);

	InitListCtrl();
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
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTessgridBuilderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
// �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
// �⽫�ɿ���Զ���ɡ�

void CTessgridBuilderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ
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
}

void CTessgridBuilderDlg::OnOpenFile()
{
	CString defaultPath = Root::getSingleton().getConfigManager()->getSceneConfig().ModelsPath.c_str();
	CString filter = _T("�ļ� (*.tessgrid)|*.tessgrid||");
	CFileDialog dlg(TRUE, defaultPath, _T(""), OFN_HIDEREADONLY | OFN_READONLY, filter, NULL);
	if (IDOK == dlg.DoModal())
	{
		m_treeCtrl.DeleteAllItems();
		m_isTreeCtrlExpand = false;
		m_curSelItemType = EICON_NONE;
		m_hParentModelItem = NULL;
		ResetCtrlLayout();
		CGridDataManager::GetIns().ClearAllModel();
		if (CGridDataManager::GetIns().OpenTessgridFile(dlg.GetPathName()))
			RefreshTreeCtrl();
		else
			AfxMessageBox(_T("���ļ�ʧ�ܣ�"));
	}
}

void CTessgridBuilderDlg::OnSaveFile()
{
}

void CTessgridBuilderDlg::OnSaveAs()
{
}

void CTessgridBuilderDlg::OnResetCamera()
{
	OrbitCamera* pCam = TessgridApp::getSingleton().getOrbitCamera();
	if (pCam)
	{
		pCam->lookAt(Vector3df(0, 0, 100), Vector3df::Origin, Vector3df::YAxis);
		pCam->resetArcball();
	}
}

void CTessgridBuilderDlg::OnExpandTreeCtrl()
{
	ResetCtrlLayout();
	ExpandTreeCtrl(m_treeCtrl.GetRootItem(), !m_isTreeCtrlExpand);
}

void CTessgridBuilderDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint pt = point;
	m_treeCtrl.ScreenToClient(&point);
	UINT uFlags;
	HTREEITEM hItem = m_treeCtrl.HitTest(point, &uFlags);
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		m_treeCtrl.ClientToScreen(&point);
		m_treeCtrl.Select(hItem, TVGN_CARET);
		CString itemText = m_treeCtrl.GetItemText(hItem);
		CMenu menu;
		menu.LoadMenu(IDR_MENU2);
		CMenu* subMenu = menu.GetSubMenu(0);

		if (NULL == m_treeCtrl.GetParentItem(hItem))
		{
			subMenu->RemoveMenu(0, MF_BYPOSITION);
		}
		else if (itemText == _T("Բ��") || itemText == _T("����"))
		{
			subMenu->RemoveMenu(0, MF_BYPOSITION);
		}
		else if (itemText == _T("Բ������") || itemText == _T("���������"))
		{
			subMenu->RemoveMenu(1, MF_BYPOSITION);
		}

		subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}

	CRect rc;
	m_listCtrl.ScreenToClient(&pt);
	m_listCtrl.GetClientRect(rc);
	if ((m_curSelItemType == EICON_CIRCLE || m_curSelItemType == EICON_POLYGON) && rc.PtInRect(pt))
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU3);
		CMenu* subMenu = menu.GetSubMenu(0);

		int nListItem = m_listCtrl.HitTest(pt, &uFlags);
		if (nListItem == -1)
		{
			subMenu->RemoveMenu(1, MF_BYPOSITION);
		}

		m_listCtrl.ClientToScreen(&pt);
		subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	}
}

void CTessgridBuilderDlg::OnInsertContour()
{
	HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
	if (NULL == hItem || NULL == m_treeCtrl.GetParentItem(hItem))
		return;

	CString itemText = m_treeCtrl.GetItemText(hItem);
	if (itemText == _T("Բ������"))
	{
		HTREEITEM hChildItem = m_treeCtrl.InsertItem(_T("Բ��"), EICON_CIRCLE, EICON_CIRCLE, hItem);
		m_treeCtrl.Expand(hItem, TVE_EXPAND);
		if (m_hParentModelItem)
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			if (pGridData)
			{
				pGridData->m_circles.push_back(CGridCircle());
				pGridData->m_needUpdateModel = true;
				m_treeCtrl.SetItemData(hChildItem, (DWORD_PTR)&(pGridData->m_circles.back()));
			}
		}
	}
	else if (itemText == _T("���������"))
	{
		HTREEITEM hChildItem = m_treeCtrl.InsertItem(_T("�����"), EICON_POLYGON, EICON_POLYGON, hItem);
		m_treeCtrl.Expand(hItem, TVE_EXPAND);
		if (m_hParentModelItem)
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			if (pGridData)
			{
				pGridData->m_polygons.push_back(CGridPolygon());
				pGridData->m_needUpdateModel = true;
				m_treeCtrl.SetItemData(hChildItem, (DWORD_PTR)&(pGridData->m_polygons.back()));
			}
		}
	}
	else if (itemText == _T("�����"))
	{
		HTREEITEM hChildItem = m_treeCtrl.InsertItem(_T("����"), EICON_POLYPOINT, EICON_POLYPOINT, hItem);
		m_treeCtrl.Expand(hItem, TVE_EXPAND);
		CGridPolygon* pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(hItem);
		if (pGridPoly)
		{
			pGridPoly->m_polyPts.push_back(CGridPolygon::SPolyPoint());
			if (m_hParentModelItem)
			{
				CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
				if (pGridData)
					pGridData->m_needUpdateModel = true;
			}
			m_treeCtrl.SetItemData(hChildItem, (DWORD_PTR)&(pGridPoly->m_polyPts.back()));
		}
	}
}

void CTessgridBuilderDlg::OnDeleteContour()
{
	HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
	if (hItem)
		m_treeCtrl.DeleteItem(hItem);
}

void CTessgridBuilderDlg::OnAddStretchBody()
{
	switch (m_curSelItemType)
	{
	case EICON_CIRCLE:
		if (m_curSelTreeCtrlItem != NULL)
		{
			CGridCircle* pGridCircle = (CGridCircle*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
			if (pGridCircle)
			{
				list<SpatialInfo>& refList = pGridCircle->m_shapeOp.m_stretchingBodies.spaceInfo;
				refList.push_back(SpatialInfo());
				int nItem = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), _T("����"), 0);
				m_listCtrl.SetItemData(nItem, (DWORD_PTR)&refList.back());

				CString strStretchNum;
				strStretchNum.Format(_T("�������������%d��"), m_listCtrl.GetItemCount());
				m_staStretchNum.SetWindowText(strStretchNum);
				NeedRefreshModel();
			}
		}
		break;
	case EICON_POLYGON:
		if (m_curSelTreeCtrlItem != NULL)
		{
			CGridPolygon* pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
			if (pGridPoly)
			{
				list<SpatialInfo>& refList = pGridPoly->m_shapeOp.m_stretchingBodies.spaceInfo;
				refList.push_back(SpatialInfo());
				int nItem = m_listCtrl.InsertItem(m_listCtrl.GetItemCount(), _T("����"), 0);
				m_listCtrl.SetItemData(nItem, (DWORD_PTR)&refList.back());

				CString strStretchNum;
				strStretchNum.Format(_T("�������������%d��"), m_listCtrl.GetItemCount());
				m_staStretchNum.SetWindowText(strStretchNum);
				NeedRefreshModel();
			}
		}
		break;
	}
}

void CTessgridBuilderDlg::OnRemoveStretchBody()
{
	int mark = m_listCtrl.GetSelectionMark();
	if (mark != -1)
	{
		m_listCtrlDeleteGridData = true;
		m_listCtrl.DeleteItem(mark);
		m_listCtrlDeleteGridData = false;
		CString strStretchNum;
		strStretchNum.Format(_T("�������������%d��"), m_listCtrl.GetItemCount());
		m_staStretchNum.SetWindowText(strStretchNum);
	}
}

void CTessgridBuilderDlg::OnTvnSelchangedTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_listCtrl.DeleteAllItems();
	m_curSelItemType = EICON_NONE;
	m_hParentModelItem = NULL;
	m_curSelTreeCtrlItem = NULL;
	m_curSelListCtrlItem = -1;
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (pNMTreeView && pNMTreeView->itemOld.hItem != pNMTreeView->itemNew.hItem)
	{
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		CString itemText = m_treeCtrl.GetItemText(hItem);
		if (NULL == m_treeCtrl.GetParentItem(hItem))
		{
			m_curSelItemType = EICON_MODEL;
			ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE);
			RefreshPropGridCtrl(EICON_MODEL, hItem);
		}
		else if (itemText == _T("Բ��"))
		{
			m_curSelItemType = EICON_CIRCLE;
			ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_CIRCLE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
			RefreshPropGridCtrl(EICON_CIRCLE, hItem);
		}
		else if (itemText == _T("�����"))
		{
			m_curSelItemType = EICON_POLYGON;
			ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
			RefreshPropGridCtrl(EICON_POLYGON, hItem);
		}
		else if (itemText == _T("����"))
		{
			m_curSelItemType = EICON_POLYPOINT;
			ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_POLY);
			RefreshPropGridCtrl(EICON_POLYPOINT, hItem);
		}
		else if (itemText == _T("Բ������"))
		{
			m_curSelItemType = EICON_CIRCLECONTOUR;
			ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
			RefreshPropGridCtrl(EICON_CIRCLECONTOUR, hItem);
		}
		else if (itemText == _T("���������"))
		{
			m_curSelItemType = EICON_POLYCONTOUR;
			ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
			RefreshPropGridCtrl(EICON_POLYCONTOUR, hItem);
		}

		HTREEITEM hParentItem = hItem;
		while (hItem)
		{
			hParentItem = hItem;
			hItem = m_treeCtrl.GetParentItem(hItem);
		}

		if (hParentItem)
		{
			m_hParentModelItem = hParentItem;
			CGridDataManager::GetIns().GeneratingModel(m_treeCtrl.GetItemText(hParentItem), false);
		}
	}
	
	CString strStretchNum;
	strStretchNum.Format(_T("�������������%d��"), m_listCtrl.GetItemCount());
	m_staStretchNum.SetWindowText(strStretchNum);
	*pResult = 0;
}

void CTessgridBuilderDlg::OnTvnDeleteitemTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	if (pNMTreeView && pNMTreeView->itemOld.hItem)
	{
		HTREEITEM hItem = pNMTreeView->itemOld.hItem;
		DWORD_PTR itemData = m_treeCtrl.GetItemData(hItem);
		CString itemText = m_treeCtrl.GetItemText(hItem);
		if (NULL == m_treeCtrl.GetParentItem(hItem))
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

			ShowPropGridCtrl(EPGC_HIDE_ALL);
			CGridDataManager::GetIns().HideOtherModel();
		}
		else if (itemText == _T("Բ��"))
		{
			CGridCircle* pGridCircle = (CGridCircle*)itemData;
			HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hItem);
			if (pGridCircle && hParentItem)
			{
				CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(hParentItem);
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
		else if (itemText == _T("�����"))
		{
			CGridPolygon* pGridPoly = (CGridPolygon*)itemData;
			HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hItem);
			if (pGridPoly && hParentItem)
			{
				CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(hParentItem);
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
		else if (itemText == _T("����"))
		{
			CGridPolygon::SPolyPoint* pPolyPt = (CGridPolygon::SPolyPoint*)itemData;
			HTREEITEM hParentItem = m_treeCtrl.GetParentItem(hItem);
			if (pPolyPt && hParentItem)
			{
				CGridPolygon* pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(hParentItem);
				if (pGridPoly)
				{
					list<CGridPolygon::SPolyPoint>::iterator it = pGridPoly->m_polyPts.begin();
					for (; it != pGridPoly->m_polyPts.end(); ++it)
					{
						if (&(*it) == pPolyPt)
						{
							it = pGridPoly->m_polyPts.erase(it);
							if (m_hParentModelItem)
							{
								CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
								if (pGridData)
									pGridData->m_needUpdateModel = true;
							}
							break;
						}
					}
				}
			}
		}
		else if (itemText == _T("Բ������"))
		{
			CGridData* pGridData = (CGridData*)itemData;
			if (pGridData)
			{
				pGridData->m_circles.clear();
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (itemText == _T("���������"))
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
	CGridCircle* pGridCircle;
	CGridPolygon* pGridPoly;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV && pNMLV->uNewState != 0 && pNMLV->uOldState != pNMLV->uNewState)
	{
		switch (m_curSelItemType)
		{
		case EICON_CIRCLE:
			if (m_curSelListCtrlItem != pNMLV->iItem)
			{
				m_curSelListCtrlItem = pNMLV->iItem;
				ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_CIRCLE | EPGC_STRETCH | EPGC_TOPCAP | EPGC_BOTTOMCAP);
				if (m_curSelTreeCtrlItem != NULL)
				{
					pGridCircle = (CGridCircle*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
					if (pGridCircle)
						SetStretchBodyGridCtrlValue(pGridCircle->m_shapeOp.m_stretchingBodies, pNMLV->iItem);
				}
			}
			break;
		case EICON_POLYGON:
			if (m_curSelListCtrlItem != pNMLV->iItem)
			{
				m_curSelListCtrlItem = pNMLV->iItem;
				ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_STRETCH | EPGC_TOPCAP | EPGC_BOTTOMCAP);
				if (m_curSelTreeCtrlItem != NULL)
				{
					pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
					if (pGridPoly)
						SetStretchBodyGridCtrlValue(pGridPoly->m_shapeOp.m_stretchingBodies, pNMLV->iItem);
				}
			}
			break;
		default:
			break;
		}
	}
	else if (pNMLV && pNMLV->uNewState == 0)
	{
		switch (m_curSelItemType)
		{
		case EICON_CIRCLE:
			m_curSelListCtrlItem = -1;
			ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_CIRCLE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
			break;
		case EICON_POLYGON:
			m_curSelListCtrlItem = -1;
			ShowPropGridCtrl(EPGC_PROP | EPGC_PLACE | EPGC_TOPCAP | EPGC_BOTTOMCAP);
			break;
		}
	}

	*pResult = 0;
}

void CTessgridBuilderDlg::OnLvnDeleteitemListContourIdx(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (m_listCtrlDeleteGridData && pNMLV && pNMLV->iItem != -1)
	{
		SpatialInfo* pSpatialInfo = (SpatialInfo*)m_listCtrl.GetItemData(pNMLV->iItem);
		if (pSpatialInfo)
		{
			switch (m_curSelItemType)
			{
			case EICON_CIRCLE:
				if (m_curSelTreeCtrlItem != NULL)
				{
					CGridCircle* pGridCircle = (CGridCircle*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
					if (pGridCircle)
					{
						list<SpatialInfo>& refList = pGridCircle->m_shapeOp.m_stretchingBodies.spaceInfo;
						list<SpatialInfo>::const_iterator it = std::find_if(refList.cbegin(), refList.cend(), GeneCampare(pSpatialInfo));
						if (it != refList.cend())
						{
							refList.erase(it);
							NeedRefreshModel();
						}
					}
				}
				break;
			case EICON_POLYGON:
				if (m_curSelTreeCtrlItem != NULL)
				{
					CGridPolygon* pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
					if (pGridPoly)
					{
						list<SpatialInfo>& refList = pGridPoly->m_shapeOp.m_stretchingBodies.spaceInfo;
						list<SpatialInfo>::const_iterator it = std::find_if(refList.cbegin(), refList.cend(), GeneCampare(pSpatialInfo));
						if (it != refList.cend())
						{
							refList.erase(it);
							NeedRefreshModel();
						}
					}
				}
				break;
			}
		}
	}
	*pResult = 0;
}

LRESULT CTessgridBuilderDlg::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pGridPropCtrl = (CMFCPropertyGridProperty*)lParam;
	CString parentPropName = pGridPropCtrl->GetParent() ? pGridPropCtrl->GetParent()->GetName() : _T("");
	CString propName = pGridPropCtrl->GetName();
	CString propValue = pGridPropCtrl->GetValue();
	if (parentPropName == _T("����"))
	{
		if (propName == _T("ģ������"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			if (pGridData && propValue != pGridData->m_prop.m_modelName)
			{
				pGridData->m_prop.m_modelName = propValue;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("��������"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			if (pGridData && propValue != pGridData->m_prop.m_materialName)
			{
				pGridData->m_prop.m_materialName = propValue;
				pGridData->m_needUpdateModel = true;
			}
		}
	}
	else if (parentPropName == _T("λ��"))
	{
		if (propName == _T("��׼ƽ��"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			if (pGridData && propValue != pGridData->m_place.m_plane)
			{
				pGridData->m_place.m_plane = propValue;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("X��ƽ��"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._translation._x))
			{
				pGridData->m_place.m_spaceInfo._translation._x = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Y��ƽ��"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._translation._y))
			{
				pGridData->m_place.m_spaceInfo._translation._y = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Z��ƽ��"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._translation._z))
			{
				pGridData->m_place.m_spaceInfo._translation._z = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("X����ת"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._rotation._x))
			{
				pGridData->m_place.m_spaceInfo._rotation._x = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Y����ת"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._rotation._y))
			{
				pGridData->m_place.m_spaceInfo._rotation._y = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Z����ת"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._rotation._z))
			{
				pGridData->m_place.m_spaceInfo._rotation._z = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("X������"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._scale._x))
			{
				pGridData->m_place.m_spaceInfo._scale._x = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Y������"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._scale._y))
			{
				pGridData->m_place.m_spaceInfo._scale._y = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Z������"))
		{
			CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._scale._z))
			{
				pGridData->m_place.m_spaceInfo._scale._z = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
	}
	else if (parentPropName == _T("Բ��"))
	{
		if (propName == _T("ֱ��"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridCir && !equals(fVal, pGridCir->m_diameter))
			{
				pGridCir->m_diameter = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Բ��X"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridCir && !equals(fVal, pGridCir->m_pos._x))
			{
				pGridCir->m_pos._x = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Բ��Y"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridCir && !equals(fVal, pGridCir->m_pos._y))
			{
				pGridCir->m_pos._y = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("������"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_treeCtrl.GetItemData(hItem);
			unsigned int uVal = (unsigned int)_wtoi(propValue.GetString());
			if (pGridCir && uVal != pGridCir->m_cnt)
			{
				pGridCir->m_cnt = uVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("˳ʱ��"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_treeCtrl.GetItemData(hItem);
			bool bVal = _wtoi(propValue.GetString()) != 0;
			if (pGridCir && bVal != pGridCir->m_clockwise)
			{
				pGridCir->m_clockwise = bVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
	}
	else if (parentPropName == _T("����"))
	{
		if (propName == _T("������X"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridPolyPt && !equals(fVal, pGridPolyPt->m_pos._x))
			{
				pGridPolyPt->m_pos._x = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("������Y"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridPolyPt && !equals(fVal, pGridPolyPt->m_pos._y))
			{
				pGridPolyPt->m_pos._y = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("�Ƕ�"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridPolyPt && !equals(fVal, pGridPolyPt->m_degree))
			{
				pGridPolyPt->m_degree = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("������"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_treeCtrl.GetItemData(hItem);
			unsigned int uVal = (unsigned int)_wtoi(propValue.GetString());
			if (pGridPolyPt && uVal != pGridPolyPt->m_cnt)
			{
				pGridPolyPt->m_cnt = uVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("˳ʱ��"))
		{
			HTREEITEM hItem = m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_treeCtrl.GetItemData(hItem);
			bool bVal = _wtoi(propValue.GetString()) != 0;
			if (pGridPolyPt && bVal != pGridPolyPt->m_clockwise)
			{
				pGridPolyPt->m_clockwise = bVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
	}
	else if (parentPropName == _T("����"))
	{
		if (propName == _T("����������"))
		{
			CGridCircle* pGridCircle;
			CGridPolygon* pGridPoly;
			switch (m_curSelItemType)
			{
			case EICON_CIRCLE:
				if (m_curSelTreeCtrlItem != NULL)
				{
					pGridCircle = (CGridCircle*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
					if (pGridCircle)
					{
						pGridCircle->m_shapeOp.m_stretchingBodies.materialName = propValue;
						if (m_hParentModelItem)
						{
							CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
							if (pGridData)
								pGridData->m_needUpdateModel = true;
						}
					}
				}
				break;
			case EICON_POLYGON:
				if (m_curSelTreeCtrlItem != NULL)
				{
					pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
					if (pGridPoly)
					{
						pGridPoly->m_shapeOp.m_stretchingBodies.materialName = propValue;
						if (m_hParentModelItem)
						{
							CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
							if (pGridData)
								pGridData->m_needUpdateModel = true;
						}
					}
				}
				break;
			default:
				break;
			}
		}
		else if (propName == _T("˳ʱ��"))
		{
			CGridCircle* pGridCircle;
			CGridPolygon* pGridPoly;
			switch (m_curSelItemType)
			{
			case EICON_CIRCLE:
				if (m_curSelTreeCtrlItem != NULL)
				{
					pGridCircle = (CGridCircle*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
					if (pGridCircle)
					{
						pGridCircle->m_shapeOp.m_stretchingBodies.reverse = _wtoi(propValue.GetString()) != 0;
						if (m_hParentModelItem)
						{
							CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
							if (pGridData)
								pGridData->m_needUpdateModel = true;
						}
					}
				}
				break;
			case EICON_POLYGON:
				if (m_curSelTreeCtrlItem != NULL)
				{
					pGridPoly = (CGridPolygon*)m_treeCtrl.GetItemData(m_curSelTreeCtrlItem);
					if (pGridPoly)
					{
						pGridPoly->m_shapeOp.m_stretchingBodies.reverse = _wtoi(propValue.GetString()) != 0;
						if (m_hParentModelItem)
						{
							CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
							if (pGridData)
								pGridData->m_needUpdateModel = true;
						}
					}
				}
				break;
			default:
				break;
			}
		}
		else if (propName == _T("X��ƽ��"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_translation._x))
			{
				pSpaceData->_translation._x = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Y��ƽ��"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_translation._y))
			{
				pSpaceData->_translation._y = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Z��ƽ��"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_translation._z))
			{
				pSpaceData->_translation._z = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("X����ת"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_rotation._x))
			{
				pSpaceData->_rotation._x = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Y����ת"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_rotation._y))
			{
				pSpaceData->_rotation._y = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Z����ת"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_rotation._z))
			{
				pSpaceData->_rotation._z = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("X������"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_scale._x))
			{
				pSpaceData->_scale._x = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Y������"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_scale._y))
			{
				pSpaceData->_scale._y = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Z������"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_scale._z))
			{
				pSpaceData->_scale._z = fVal;
				if (m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
	}
	else if (parentPropName == _T("�Ƕ�"))
	{
		if (!propName.IsEmpty())
			TopCapPropertyChanged(propName, propValue);
	}
	else if (parentPropName == _T("�ǵ�"))
	{
		if (!propName.IsEmpty())
			BottomCapPropertyChanged(propName, propValue);
	}
	return 0;
}
