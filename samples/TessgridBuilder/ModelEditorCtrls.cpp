#include "stdafx.h"
#include "ModelEditorCtrls.h"
#include "TessgridBuilder.h"
#include "GridDataManager.h"
#include "TessgridBuilderDlg.h"

CModelEditorCtrls::CModelEditorCtrls()
	: m_pMainDlg(NULL)
	, m_isTreeCtrlExpand(false)
	, m_activeSelchangedAfxMsg(true)
	, m_treeCtrlDeleteGridData(false)
	, m_listCtrlDeleteGridData(false)
	, m_curSelItemType(EICON_NONE)
	, m_hParentModelItem(NULL)
	, m_curSelTreeCtrlItem(NULL)
	, m_curSelListCtrlItem(-1)
{
}

void CModelEditorCtrls::DisplayModel()
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

bool CModelEditorCtrls::Init(CTessgridBuilderDlg* dlg)
{
	m_pMainDlg = dlg;
	m_staPropGridBg.Init(dlg);
	m_staPropGridBg.ShowWindow(SW_HIDE);
	m_listCtrl.ShowWindow(SW_HIDE);
	m_treeCtrl.ShowWindow(SW_HIDE);

	if (m_listImg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2))
	{
		COLORREF co = GetSysColor(COLOR_WINDOW);
		m_listImg.SetBkColor(co);
		m_listCtrl.SetImageList(&m_listImg, LVSIL_SMALL);

		HICON hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_STRETCH), IMAGE_ICON, 16, 16, 0);
		m_listImg.Add(hIcon);

		m_listCtrl.SetFont(&afxGlobalData.fontRegular);
		m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_COLUMNOVERFLOW);
		m_listCtrl.InsertColumn(0, _T("无形体拉伸"), LVCFMT_CENTER, 192, 0);
		m_listCtrl.SetColumnWidth(0, 192);
		LVCOLUMN lvcol;
		lvcol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE;
		lvcol.fmt = LVCFMT_CENTER;
		lvcol.iImage = 0;
		lvcol.cchTextMax = 256;
		lvcol.cx = 192;
		lvcol.pszText = _T("无形体拉伸");
		m_listCtrl.SetColumn(0, &lvcol);
	}

	if (m_treeImg.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 2))
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

	m_propGridCtrl.SetParent(&m_staPropGridBg);
	m_gridCtrlItem.cxy = 120;
	m_gridCtrlItem.mask = HDI_WIDTH;
	m_propGridCtrl.GetHeaderCtrl().SetItem(0, &m_gridCtrlItem);
	m_propGridCtrl.EnableHeaderCtrl(FALSE);
	m_propGridCtrl.EnableDescriptionArea();
	m_propGridCtrl.SetVSDotNetLook();
	m_propGridCtrl.MarkModifiedProperties();

	COleVariant oleVarStr, oleVarFlt, oleVarUint, oleVarBool;
	oleVarStr.SetString(_T(""), VT_BSTR);
	oleVarFlt.ChangeType(VT_R4);
	oleVarUint.ChangeType(VT_UINT);
	oleVarBool.ChangeType(VT_BOOL);

	CMFCPropertyGridProperty* pGroupProp = new CMFCPropertyGridProperty(_T("属性"));
	m_propGridCtrl.AddProperty(pGroupProp);
	CMFCPropertyGridProperty* pChildProp = new CMFCPropertyGridProperty(_T("模型名称"), oleVarStr, _T("当前所选择的ManualObject对象名称"));
	pGroupProp->AddSubItem(pChildProp);
	pChildProp->AllowEdit(FALSE);
	CMFCPropertyGridProperty* pValueProp = new CMFCPropertyGridProperty(_T("材质名称"), oleVarStr, _T("所选ManualObject对象的公共材质"));
	pGroupProp->AddSubItem(pValueProp);

	pGroupProp = new CMFCPropertyGridProperty(_T("位置"));
	m_propGridCtrl.AddProperty(pGroupProp);
	oleVarStr.SetString(_T("XY平面"), VT_BSTR);
	pValueProp = new CMFCPropertyGridProperty(_T("基准平面"), oleVarStr, _T("模型创建所基于的二维平面"));
	pGroupProp->AddSubItem(pValueProp);
	pValueProp->AllowEdit(FALSE);
	pValueProp->AddOption(_T("XY平面"));
	pValueProp->AddOption(_T("XZ平面"));
	pValueProp->AddOption(_T("YZ平面"));
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

	pGroupProp = new CMFCPropertyGridProperty(_T("圆形"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("直径"), oleVarFlt, _T("圆的直径")));
	oleVarFlt.fltVal = 0.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("圆心X"), oleVarFlt, _T("圆心坐标X")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("圆心Y"), oleVarFlt, _T("圆心坐标Y")));
	oleVarUint.uintVal = 60u;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("采样点"), oleVarUint, _T("组成圆的顶点数量")));
	oleVarBool.boolVal = FALSE;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("顺时针"), oleVarBool, _T("圆的顶点环绕方式")));
	pGroupProp = new CMFCPropertyGridProperty(_T("顶点"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("点坐标X"), oleVarFlt, _T("组成多边形的单个顶点的X坐标")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("点坐标Y"), oleVarFlt, _T("组成多边形的单个顶点的Y坐标")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("角度"), oleVarFlt, _T("多边形上相邻两个顶点之间的边的弧度，用角度值度量")));
	oleVarUint.uintVal = 1u;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("采样点"), oleVarUint, _T("组成一条边的顶点数量")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("顺时针"), oleVarBool, _T("多边形的顶点环绕方式")));

	pGroupProp = new CMFCPropertyGridProperty(_T("拉伸"));
	m_propGridCtrl.AddProperty(pGroupProp);
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

	pGroupProp = new CMFCPropertyGridProperty(_T("盖顶"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("激活盖顶"), oleVarBool, _T("是否拥有顶部盖子")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标X"), oleVarFlt, _T("顶部平面网格的法线X坐标")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标Y"), oleVarFlt, _T("顶部平面网格的法线Y坐标")));
	oleVarFlt.fltVal = 1.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标Z"), oleVarFlt, _T("顶部平面网格的法线Z坐标")));
	oleVarStr.SetString(_T(""), VT_BSTR);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("盖顶材质"), oleVarStr, _T("顶部平面网格的材质")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("顺时针"), oleVarBool, _T("组成顶部平面网格的轮廓顶点环绕方式")));
	pGroupProp = new CMFCPropertyGridProperty(_T("盖底"));
	m_propGridCtrl.AddProperty(pGroupProp);
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("激活盖底"), oleVarBool, _T("是否拥有底部盖子")));
	oleVarFlt.fltVal = 0.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标X"), oleVarFlt, _T("底部平面网格的法线X坐标")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标Y"), oleVarFlt, _T("底部平面网格的法线Y坐标")));
	oleVarFlt.fltVal = -1.0f;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("法线坐标Z"), oleVarFlt, _T("底部平面网格的法线Z坐标")));
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("盖底材质"), oleVarStr, _T("底部平面网格的材质")));
	oleVarBool.boolVal = TRUE;
	pGroupProp->AddSubItem(new CMFCPropertyGridProperty(_T("顺时针"), oleVarBool, _T("组成底部平面网格的轮廓顶点环绕方式")));

	ShowPropGridCtrl(EPGC_HIDE_ALL);
	return true;
}

void CModelEditorCtrls::RefreshTreeCtrl()
{
	m_treeCtrl.DeleteAllItems();
	const std::list<CGridData>& gridDataset = CGridDataManager::GetIns().GetGridDataset();
	std::list<CGridData>::const_iterator cit = gridDataset.cbegin();
	for (; cit != gridDataset.cend(); ++cit)
	{
		const CGridData* pGridData = &(*cit);
		if (!pGridData)
			continue;

		HTREEITEM rootTreeItem = m_treeCtrl.InsertItem(pGridData->m_model.m_modelName, EICON_MODEL, EICON_MODEL, NULL);
		m_treeCtrl.SetItemData(rootTreeItem, (DWORD_PTR)pGridData);
		HTREEITEM treeItem = m_treeCtrl.InsertItem(_T("圆形轮廓"), EICON_CIRCLECONTOUR, EICON_CIRCLECONTOUR, rootTreeItem);
		m_treeCtrl.SetItemData(treeItem, (DWORD_PTR)pGridData);

		list<CGridCircle>::const_iterator cirIt = pGridData->m_circles.cbegin();
		for (; cirIt != pGridData->m_circles.cend(); ++cirIt)
		{
			m_treeCtrl.SetItemData(m_treeCtrl.InsertItem(_T("圆形"), EICON_CIRCLE, EICON_CIRCLE, treeItem), (DWORD_PTR)&(*cirIt));
		}

		treeItem = m_treeCtrl.InsertItem(_T("多边形轮廓"), EICON_POLYCONTOUR, EICON_POLYCONTOUR, rootTreeItem);
		m_treeCtrl.SetItemData(treeItem, (DWORD_PTR)pGridData);
		list<CGridPolygon>::const_iterator polyIt = pGridData->m_polygons.cbegin();
		for (; polyIt != pGridData->m_polygons.cend(); ++polyIt)
		{
			HTREEITEM childItem = m_treeCtrl.InsertItem(_T("多边形"), EICON_POLYGON, EICON_POLYGON, treeItem);
			m_treeCtrl.SetItemData(childItem, (DWORD_PTR)&(*polyIt));
			const list<CGridPolygon::SPolyPoint>& polyPts = (*polyIt).m_polyPts;
			list<CGridPolygon::SPolyPoint>::const_iterator ptIt = polyPts.cbegin();
			for (; ptIt != polyPts.cend(); ++ptIt)
			{
				HTREEITEM ptItem = m_treeCtrl.InsertItem(_T("顶点"), EICON_POLYPOINT, EICON_POLYPOINT, childItem);
				m_treeCtrl.SetItemData(ptItem, (DWORD_PTR)&(*ptIt));
			}
		}
	}
}

void CModelEditorCtrls::ExpandTreeCtrl(HTREEITEM treeItem, bool expanding)
{
	while (treeItem)
	{
		m_isTreeCtrlExpand = expanding;
		UINT nCode = expanding ? TVE_EXPAND : TVE_COLLAPSE;
		m_treeCtrl.Expand(treeItem, nCode);
		ExpandTreeCtrl(m_treeCtrl.GetChildItem(treeItem), expanding);
		treeItem = m_treeCtrl.GetNextSiblingItem(treeItem);
	}
}

void CModelEditorCtrls::ShowPropGridCtrl(int gridCtrlShowState)
{
	if (gridCtrlShowState == EPGC_HIDE_ALL)
	{
		int num = m_propGridCtrl.GetPropertyCount();
		for (int i = 0; i < num; ++i)
			RecursionShowPropGridCtrl(FALSE, m_propGridCtrl.GetProperty(i));
	}
	else
	{
		BitState showState = gridCtrlShowState;
		RecursionShowPropGridCtrl(showState.checkState(EPGC_PROP), m_propGridCtrl.GetProperty(0));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_PLACE), m_propGridCtrl.GetProperty(1));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_CIRCLE), m_propGridCtrl.GetProperty(2));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_POLY), m_propGridCtrl.GetProperty(3));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_STRETCH), m_propGridCtrl.GetProperty(4));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_TOPCAP), m_propGridCtrl.GetProperty(5));
		RecursionShowPropGridCtrl(showState.checkState(EPGC_BOTTOMCAP), m_propGridCtrl.GetProperty(6));
	}
}

void CModelEditorCtrls::RefreshPropGridCtrl(ETreeCtrlItemType itemType, HTREEITEM hItem)
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

		m_propGridCtrl.GetProperty(2)->GetSubItem(0)->SetValue(pGridCircle->m_diameter);
		m_propGridCtrl.GetProperty(2)->GetSubItem(1)->SetValue(pGridCircle->m_pos._x);
		m_propGridCtrl.GetProperty(2)->GetSubItem(2)->SetValue(pGridCircle->m_pos._y);
		m_propGridCtrl.GetProperty(2)->GetSubItem(3)->SetValue(_variant_t(pGridCircle->m_cnt));
		m_propGridCtrl.GetProperty(2)->GetSubItem(4)->SetValue(_variant_t(pGridCircle->m_clockwise));

		m_curSelTreeCtrlItem = hItem;

		{
			const list<SpatialInfo>& spatialInfos = pGridCircle->m_shapeOp.m_stretchingBodies.spaceInfo;
			list<SpatialInfo>::const_iterator stretchIt = spatialInfos.cbegin();
			for (int i = 0; stretchIt != spatialInfos.cend(); ++stretchIt, ++i)
			{
				m_listCtrl.InsertItem(i, _T("拉伸形体"), 0);
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
				m_listCtrl.InsertItem(i, _T("拉伸形体"), 0);
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

		m_propGridCtrl.GetProperty(3)->GetSubItem(0)->SetValue(pPolyPt->m_pos._x);
		m_propGridCtrl.GetProperty(3)->GetSubItem(1)->SetValue(pPolyPt->m_pos._y);
		m_propGridCtrl.GetProperty(3)->GetSubItem(2)->SetValue(pPolyPt->m_degree);
		m_propGridCtrl.GetProperty(3)->GetSubItem(3)->SetValue(_variant_t(pPolyPt->m_cnt));
		m_propGridCtrl.GetProperty(3)->GetSubItem(4)->SetValue(_variant_t(pPolyPt->m_clockwise));

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

void CModelEditorCtrls::NeedRefreshModel()
{
	if (m_hParentModelItem)
	{
		CGridData* pGridData = (CGridData*)m_treeCtrl.GetItemData(m_hParentModelItem);
		if (pGridData)
			pGridData->m_needUpdateModel = true;
	}
}

void CModelEditorCtrls::ResetCtrlLayout()
{
	LVCOLUMN lvcol;
	lvcol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_IMAGE;
	lvcol.fmt = LVCFMT_CENTER;
	lvcol.iImage = 0;
	lvcol.cchTextMax = 256;
	lvcol.cx = 192;
	lvcol.pszText = _T("无形体拉伸");
	m_listCtrl.SetColumn(0, &lvcol);
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

void CModelEditorCtrls::RecursionShowPropGridCtrl(BOOL bShow, CMFCPropertyGridProperty* pGridProperty)
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

bool CModelEditorCtrls::SetPropAndPlaceGridCtrlValue(const CGridData* pGridData)
{
	if (!pGridData)
		return false;

	m_propGridCtrl.GetProperty(0)->GetSubItem(0)->SetValue(pGridData->m_model.m_modelName.GetString());
	m_propGridCtrl.GetProperty(1)->GetSubItem(0)->SetValue(pGridData->m_place.m_plane.GetString());
	m_propGridCtrl.GetProperty(1)->GetSubItem(1)->SetValue(pGridData->m_place.m_spaceInfo._translation._x);
	m_propGridCtrl.GetProperty(1)->GetSubItem(2)->SetValue(pGridData->m_place.m_spaceInfo._translation._y);
	m_propGridCtrl.GetProperty(1)->GetSubItem(3)->SetValue(pGridData->m_place.m_spaceInfo._translation._z);
	m_propGridCtrl.GetProperty(1)->GetSubItem(4)->SetValue(pGridData->m_place.m_spaceInfo._rotation._x);
	m_propGridCtrl.GetProperty(1)->GetSubItem(5)->SetValue(pGridData->m_place.m_spaceInfo._rotation._y);
	m_propGridCtrl.GetProperty(1)->GetSubItem(6)->SetValue(pGridData->m_place.m_spaceInfo._rotation._z);
	m_propGridCtrl.GetProperty(1)->GetSubItem(7)->SetValue(pGridData->m_place.m_spaceInfo._scale._x);
	m_propGridCtrl.GetProperty(1)->GetSubItem(8)->SetValue(pGridData->m_place.m_spaceInfo._scale._y);
	m_propGridCtrl.GetProperty(1)->GetSubItem(9)->SetValue(pGridData->m_place.m_spaceInfo._scale._z);

	return true;
}

bool CModelEditorCtrls::SetTopAndBottomCapGridCtrlValue(const CGridData* pGridData)
{
	if (!pGridData)
		return false;

	m_propGridCtrl.GetProperty(5)->GetSubItem(0)->SetValue(_variant_t(pGridData->m_isTopCapTessAll));
	m_propGridCtrl.GetProperty(5)->GetSubItem(1)->SetValue(pGridData->m_topCap.normal._x);
	m_propGridCtrl.GetProperty(5)->GetSubItem(1)->Enable(pGridData->m_isTopCapTessAll);
	m_propGridCtrl.GetProperty(5)->GetSubItem(2)->SetValue(pGridData->m_topCap.normal._y);
	m_propGridCtrl.GetProperty(5)->GetSubItem(2)->Enable(pGridData->m_isTopCapTessAll);
	m_propGridCtrl.GetProperty(5)->GetSubItem(3)->SetValue(pGridData->m_topCap.normal._z);
	m_propGridCtrl.GetProperty(5)->GetSubItem(3)->Enable(pGridData->m_isTopCapTessAll);
	m_propGridCtrl.GetProperty(5)->GetSubItem(4)->SetValue(pGridData->m_topCap.materialName.GetString());
	m_propGridCtrl.GetProperty(5)->GetSubItem(4)->Enable(pGridData->m_isTopCapTessAll);
	m_propGridCtrl.GetProperty(5)->GetSubItem(5)->SetValue(_variant_t(pGridData->m_topCap.reverse));
	m_propGridCtrl.GetProperty(5)->GetSubItem(5)->Enable(pGridData->m_isTopCapTessAll);

	m_propGridCtrl.GetProperty(6)->GetSubItem(0)->SetValue(_variant_t(pGridData->m_isBottomCapTessAll));
	m_propGridCtrl.GetProperty(6)->GetSubItem(1)->SetValue(pGridData->m_bottomCap.normal._x);
	m_propGridCtrl.GetProperty(6)->GetSubItem(1)->Enable(pGridData->m_isBottomCapTessAll);
	m_propGridCtrl.GetProperty(6)->GetSubItem(2)->SetValue(pGridData->m_bottomCap.normal._y);
	m_propGridCtrl.GetProperty(6)->GetSubItem(2)->Enable(pGridData->m_isBottomCapTessAll);
	m_propGridCtrl.GetProperty(6)->GetSubItem(3)->SetValue(pGridData->m_bottomCap.normal._z);
	m_propGridCtrl.GetProperty(6)->GetSubItem(3)->Enable(pGridData->m_isBottomCapTessAll);
	m_propGridCtrl.GetProperty(6)->GetSubItem(4)->SetValue(pGridData->m_bottomCap.materialName.GetString());
	m_propGridCtrl.GetProperty(6)->GetSubItem(4)->Enable(pGridData->m_isBottomCapTessAll);
	m_propGridCtrl.GetProperty(6)->GetSubItem(5)->SetValue(_variant_t(pGridData->m_bottomCap.reverse));
	m_propGridCtrl.GetProperty(6)->GetSubItem(5)->Enable(pGridData->m_isBottomCapTessAll);

	return true;
}

void CModelEditorCtrls::SetTopAndBottomCapGridCtrlValue(const CGridShapeOp& shapeOp)
{
	m_propGridCtrl.GetProperty(5)->GetSubItem(0)->SetValue(_variant_t(shapeOp.m_hasTopCaps));
	m_propGridCtrl.GetProperty(5)->GetSubItem(1)->SetValue(shapeOp.m_topCap.normal._x);
	m_propGridCtrl.GetProperty(5)->GetSubItem(1)->Enable(shapeOp.m_hasTopCaps);
	m_propGridCtrl.GetProperty(5)->GetSubItem(2)->SetValue(shapeOp.m_topCap.normal._y);
	m_propGridCtrl.GetProperty(5)->GetSubItem(2)->Enable(shapeOp.m_hasTopCaps);
	m_propGridCtrl.GetProperty(5)->GetSubItem(3)->SetValue(shapeOp.m_topCap.normal._z);
	m_propGridCtrl.GetProperty(5)->GetSubItem(3)->Enable(shapeOp.m_hasTopCaps);
	m_propGridCtrl.GetProperty(5)->GetSubItem(4)->SetValue(shapeOp.m_topCap.materialName.GetString());
	m_propGridCtrl.GetProperty(5)->GetSubItem(4)->Enable(shapeOp.m_hasTopCaps);
	m_propGridCtrl.GetProperty(5)->GetSubItem(5)->SetValue(_variant_t(shapeOp.m_topCap.reverse));
	m_propGridCtrl.GetProperty(5)->GetSubItem(5)->Enable(shapeOp.m_hasTopCaps);

	m_propGridCtrl.GetProperty(6)->GetSubItem(0)->SetValue(_variant_t(shapeOp.m_hasBottomCaps));
	m_propGridCtrl.GetProperty(6)->GetSubItem(1)->SetValue(shapeOp.m_bottomCap.normal._x);
	m_propGridCtrl.GetProperty(6)->GetSubItem(1)->Enable(shapeOp.m_hasBottomCaps);
	m_propGridCtrl.GetProperty(6)->GetSubItem(2)->SetValue(shapeOp.m_bottomCap.normal._y);
	m_propGridCtrl.GetProperty(6)->GetSubItem(2)->Enable(shapeOp.m_hasBottomCaps);
	m_propGridCtrl.GetProperty(6)->GetSubItem(3)->SetValue(shapeOp.m_bottomCap.normal._z);
	m_propGridCtrl.GetProperty(6)->GetSubItem(3)->Enable(shapeOp.m_hasBottomCaps);
	m_propGridCtrl.GetProperty(6)->GetSubItem(4)->SetValue(shapeOp.m_bottomCap.materialName.GetString());
	m_propGridCtrl.GetProperty(6)->GetSubItem(4)->Enable(shapeOp.m_hasBottomCaps);
	m_propGridCtrl.GetProperty(6)->GetSubItem(5)->SetValue(_variant_t(shapeOp.m_bottomCap.reverse));
	m_propGridCtrl.GetProperty(6)->GetSubItem(5)->Enable(shapeOp.m_hasBottomCaps);
}

void CModelEditorCtrls::SetStretchBodyGridCtrlValue(const CGridShapeOp::SStretchingBody& shapeOpStretchBody, int iItem)
{
	m_propGridCtrl.GetProperty(4)->GetSubItem(0)->SetValue(shapeOpStretchBody.materialName.GetString());
	m_propGridCtrl.GetProperty(4)->GetSubItem(1)->SetValue(_variant_t(shapeOpStretchBody.reverse));
	if (iItem != -1)
	{
		SpatialInfo* pSpaceData = (SpatialInfo*)m_listCtrl.GetItemData(iItem);
		if (pSpaceData)
		{
			m_propGridCtrl.GetProperty(4)->GetSubItem(2)->SetValue(pSpaceData->_translation._x);
			m_propGridCtrl.GetProperty(4)->GetSubItem(3)->SetValue(pSpaceData->_translation._y);
			m_propGridCtrl.GetProperty(4)->GetSubItem(4)->SetValue(pSpaceData->_translation._z);
			m_propGridCtrl.GetProperty(4)->GetSubItem(5)->SetValue(pSpaceData->_rotation._x);
			m_propGridCtrl.GetProperty(4)->GetSubItem(6)->SetValue(pSpaceData->_rotation._y);
			m_propGridCtrl.GetProperty(4)->GetSubItem(7)->SetValue(pSpaceData->_rotation._z);
			m_propGridCtrl.GetProperty(4)->GetSubItem(8)->SetValue(pSpaceData->_scale._x);
			m_propGridCtrl.GetProperty(4)->GetSubItem(9)->SetValue(pSpaceData->_scale._y);
			m_propGridCtrl.GetProperty(4)->GetSubItem(10)->SetValue(pSpaceData->_scale._z);
		}
	}
}

void CModelEditorCtrls::TopCapPropertyChanged(CString propName, CString propValue)
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
				if (propName == _T("激活盖顶"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridCircle->m_shapeOp.m_hasTopCaps)
					{
						pGridCircle->m_shapeOp.m_hasTopCaps = bVal;
						m_propGridCtrl.GetProperty(5)->GetSubItem(1)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(2)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(3)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(4)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(5)->Enable(bVal);
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_topCap.normal._x))
					{
						pGridCircle->m_shapeOp.m_topCap.normal._x = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_topCap.normal._y))
					{
						pGridCircle->m_shapeOp.m_topCap.normal._y = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_topCap.normal._z))
					{
						pGridCircle->m_shapeOp.m_topCap.normal._z = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("盖顶材质"))
				{
					if (propValue != pGridCircle->m_shapeOp.m_topCap.materialName)
					{
						pGridCircle->m_shapeOp.m_topCap.materialName = propValue;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("顺时针"))
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
				if (propName == _T("激活盖顶"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridPoly->m_shapeOp.m_hasTopCaps)
					{
						pGridPoly->m_shapeOp.m_hasTopCaps = bVal;
						m_propGridCtrl.GetProperty(5)->GetSubItem(1)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(2)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(3)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(4)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(5)->Enable(bVal);
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_topCap.normal._x))
					{
						pGridPoly->m_shapeOp.m_topCap.normal._x = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_topCap.normal._y))
					{
						pGridPoly->m_shapeOp.m_topCap.normal._y = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_topCap.normal._z))
					{
						pGridPoly->m_shapeOp.m_topCap.normal._z = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("盖顶材质"))
				{
					if (propValue != pGridPoly->m_shapeOp.m_topCap.materialName)
					{
						pGridPoly->m_shapeOp.m_topCap.materialName = propValue;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("顺时针"))
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
				if (propName == _T("激活盖顶"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridData->m_isTopCapTessAll)
					{
						pGridData->m_isTopCapTessAll = bVal;
						m_propGridCtrl.GetProperty(5)->GetSubItem(1)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(2)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(3)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(4)->Enable(bVal);
						m_propGridCtrl.GetProperty(5)->GetSubItem(5)->Enable(bVal);
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("法线坐标X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_topCap.normal._x))
					{
						pGridData->m_topCap.normal._x = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("法线坐标Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_topCap.normal._y))
					{
						pGridData->m_topCap.normal._y = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("法线坐标Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_topCap.normal._z))
					{
						pGridData->m_topCap.normal._z = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("盖顶材质"))
				{
					if (propValue != pGridData->m_topCap.materialName)
					{
						pGridData->m_topCap.materialName = propValue;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("顺时针"))
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

void CModelEditorCtrls::BottomCapPropertyChanged(CString propName, CString propValue)
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
				if (propName == _T("激活盖底"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridCircle->m_shapeOp.m_hasBottomCaps)
					{
						pGridCircle->m_shapeOp.m_hasBottomCaps = bVal;
						m_propGridCtrl.GetProperty(6)->GetSubItem(1)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(2)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(3)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(4)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(5)->Enable(bVal);
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_bottomCap.normal._x))
					{
						pGridCircle->m_shapeOp.m_bottomCap.normal._x = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_bottomCap.normal._y))
					{
						pGridCircle->m_shapeOp.m_bottomCap.normal._y = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridCircle->m_shapeOp.m_bottomCap.normal._z))
					{
						pGridCircle->m_shapeOp.m_bottomCap.normal._z = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("盖底材质"))
				{
					if (propValue != pGridCircle->m_shapeOp.m_bottomCap.materialName)
					{
						pGridCircle->m_shapeOp.m_bottomCap.materialName = propValue;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("顺时针"))
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
				if (propName == _T("激活盖底"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridPoly->m_shapeOp.m_hasBottomCaps)
					{
						pGridPoly->m_shapeOp.m_hasBottomCaps = bVal;
						m_propGridCtrl.GetProperty(6)->GetSubItem(1)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(2)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(3)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(4)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(5)->Enable(bVal);
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_bottomCap.normal._x))
					{
						pGridPoly->m_shapeOp.m_bottomCap.normal._x = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_bottomCap.normal._y))
					{
						pGridPoly->m_shapeOp.m_bottomCap.normal._y = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("法线坐标Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridPoly->m_shapeOp.m_bottomCap.normal._z))
					{
						pGridPoly->m_shapeOp.m_bottomCap.normal._z = fVal;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("盖底材质"))
				{
					if (propValue != pGridPoly->m_shapeOp.m_bottomCap.materialName)
					{
						pGridPoly->m_shapeOp.m_bottomCap.materialName = propValue;
						NeedRefreshModel();
					}
				}
				else if (propName == _T("顺时针"))
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
				if (propName == _T("激活盖底"))
				{
					bool bVal = _wtoi(propValue.GetString()) != 0;
					if (bVal != pGridData->m_isBottomCapTessAll)
					{
						pGridData->m_isBottomCapTessAll = bVal;
						m_propGridCtrl.GetProperty(6)->GetSubItem(1)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(2)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(3)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(4)->Enable(bVal);
						m_propGridCtrl.GetProperty(6)->GetSubItem(5)->Enable(bVal);
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("法线坐标X"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_bottomCap.normal._x))
					{
						pGridData->m_bottomCap.normal._x = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("法线坐标Y"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_bottomCap.normal._y))
					{
						pGridData->m_bottomCap.normal._y = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("法线坐标Z"))
				{
					float fVal = (float)_wtof(propValue.GetString());
					if (!equals(fVal, pGridData->m_bottomCap.normal._z))
					{
						pGridData->m_bottomCap.normal._z = fVal;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("盖底材质"))
				{
					if (propValue != pGridData->m_bottomCap.materialName)
					{
						pGridData->m_bottomCap.materialName = propValue;
						pGridData->m_needUpdateModel = true;
					}
				}
				else if (propName == _T("顺时针"))
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
