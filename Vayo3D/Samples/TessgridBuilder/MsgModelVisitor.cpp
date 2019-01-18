#include "stdafx.h"
#include "MsgModelVisitor.h"
#include "TessgridBuilder.h"
#include "NewModelDlg.h"
#include "GridDataManager.h"
#include "TessgridBuilderDlg.h"

CMsgModelVisitor::CMsgModelVisitor(CTessgridBuilderDlg* dlg)
	: m_pEditorCtrls(&(dlg->m_modelEditorCtrls))
{
}

CMsgModelVisitor::~CMsgModelVisitor()
{
}

void CMsgModelVisitor::OnSize(CRect clientRc, CRect tabctrlRc)
{
	CRect rc = tabctrlRc;
	rc.bottom -= tabctrlRc.Height() / 2;
	m_pEditorCtrls->m_treeCtrl.MoveWindow(rc);

	rc = CRect(rc.left, rc.bottom + 1, tabctrlRc.right, tabctrlRc.bottom);
	m_pEditorCtrls->m_listCtrl.MoveWindow(rc);

	rc = CRect(clientRc.Width() - 260, 42, clientRc.Width(), clientRc.Height() - 20);
	m_pEditorCtrls->m_staPropGridBg.MoveWindow(rc);
	m_pEditorCtrls->m_propGridCtrl.MoveWindow(CRect(0, 0, rc.Width() - 1, rc.Height() - 1));
}

void CMsgModelVisitor::ShowCtrls(bool bShow)
{
	int nCmdShow = (bShow ? SW_SHOW : SW_HIDE);
	m_pEditorCtrls->m_listCtrl.ShowWindow(nCmdShow);
	m_pEditorCtrls->m_treeCtrl.ShowWindow(nCmdShow);
	m_pEditorCtrls->m_staPropGridBg.ShowWindow(nCmdShow);
	m_pEditorCtrls->m_propGridCtrl.ShowWindow(nCmdShow);
}

void CMsgModelVisitor::OnNewBuild()
{
	CNewModelDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		CGridData gridData;
		gridData.m_prop.m_modelName = dlg.GetModelName();
		gridData.m_prop.m_materialName = dlg.GetModelMaterial();
		std::list<CGridData>& gridDataset = CGridDataManager::GetIns().GetGridDataset();
		gridDataset.push_back(gridData);
		const CGridData& refGridData = gridDataset.back();
		HTREEITEM rootTreeItem = m_pEditorCtrls->m_treeCtrl.InsertItem(refGridData.m_prop.m_modelName, EICON_MODEL, EICON_MODEL, NULL);
		m_pEditorCtrls->m_treeCtrl.SetItemData(rootTreeItem, (DWORD_PTR)&refGridData);
		HTREEITEM treeItem = m_pEditorCtrls->m_treeCtrl.InsertItem(_T("圆形轮廓"), EICON_CIRCLECONTOUR, EICON_CIRCLECONTOUR, rootTreeItem);
		m_pEditorCtrls->m_treeCtrl.SetItemData(treeItem, (DWORD_PTR)&refGridData);
		treeItem = m_pEditorCtrls->m_treeCtrl.InsertItem(_T("多边形轮廓"), EICON_POLYCONTOUR, EICON_POLYCONTOUR, rootTreeItem);
		m_pEditorCtrls->m_treeCtrl.SetItemData(treeItem, (DWORD_PTR)&refGridData);
	}
}

void CMsgModelVisitor::OnOpenFile()
{
	CString strTmp = Root::getSingleton().getConfigManager()->getSceneConfig().ModelsPath.c_str();
	CFileDialog dlg(TRUE, strTmp, _T(""), OFN_HIDEREADONLY | OFN_READONLY, _T("文件 (*.tessgrid)|*.tessgrid||"), NULL);
	if (IDOK == dlg.DoModal())
	{
		strTmp = dlg.GetPathName();
		if (CGridDataManager::GetIns().OpenTessgridFile(strTmp))
		{
			m_pEditorCtrls->m_savePath = strTmp;
			m_pEditorCtrls->m_isTreeCtrlExpand = false;
			m_pEditorCtrls->m_activeSelchangedAfxMsg = false;
			m_pEditorCtrls->m_curSelItemType = EICON_NONE;
			m_pEditorCtrls->m_hParentModelItem = NULL;
			m_pEditorCtrls->ResetCtrlLayout();
			CGridDataManager::GetIns().ClearAllModel();
			m_pEditorCtrls->RefreshTreeCtrl();
			m_pEditorCtrls->m_activeSelchangedAfxMsg = true;
		}
		else
		{
			strTmp.Format(_T("打开文件[%s]失败！"), strTmp);
			AfxMessageBox(strTmp);
		}
	}
}

void CMsgModelVisitor::OnSaveFile()
{
	CString strTmp;
	if (m_pEditorCtrls->m_savePath.IsEmpty())
	{
		strTmp = Root::getSingleton().getConfigManager()->getSceneConfig().ModelsPath.c_str();
		CFileDialog dlg(FALSE, strTmp, _T(""), OFN_HIDEREADONLY | OFN_READONLY, _T("文件 (*.tessgrid)|*.tessgrid||"), NULL);
		if (IDOK == dlg.DoModal())
		{
			m_pEditorCtrls->m_savePath = dlg.GetPathName();
			if (!CGridDataManager::GetIns().SaveTessgridFile(m_pEditorCtrls->m_savePath))
			{
				strTmp.Format(_T("保存文件[%s]失败！"), m_pEditorCtrls->m_savePath);
				AfxMessageBox(strTmp);
			}
		}
	}
	else
	{
		if (!CGridDataManager::GetIns().SaveTessgridFile(m_pEditorCtrls->m_savePath))
		{
			strTmp.Format(_T("保存文件[%s]失败！"), m_pEditorCtrls->m_savePath);
			AfxMessageBox(strTmp);
		}
	}
}

void CMsgModelVisitor::OnSaveAs()
{
	CString strTmp = Root::getSingleton().getConfigManager()->getSceneConfig().ModelsPath.c_str();
	CFileDialog dlg(FALSE, strTmp, _T(""), OFN_HIDEREADONLY | OFN_READONLY, _T("文件 (*.tessgrid)|*.tessgrid||"), NULL);
	if (IDOK == dlg.DoModal())
	{
		strTmp = dlg.GetPathName();
		if (!CGridDataManager::GetIns().SaveTessgridFile(strTmp))
		{
			strTmp.Format(_T("保存文件[%s]失败！"), strTmp);
			AfxMessageBox(strTmp);
		}
	}
}

void CMsgModelVisitor::OnExpandTreeCtrl()
{
	m_pEditorCtrls->ResetCtrlLayout();
	m_pEditorCtrls->ExpandTreeCtrl(m_pEditorCtrls->m_treeCtrl.GetRootItem(), !m_pEditorCtrls->m_isTreeCtrlExpand);
}

void CMsgModelVisitor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CPoint pt = point;
	m_pEditorCtrls->m_treeCtrl.ScreenToClient(&point);

	CRect rc;
	m_pEditorCtrls->m_listCtrl.ScreenToClient(&pt);
	m_pEditorCtrls->m_listCtrl.GetClientRect(rc);

	UINT uFlags;
	HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.HitTest(point, &uFlags);
	if ((hItem != NULL))
	{
		m_pEditorCtrls->m_treeCtrl.ClientToScreen(&point);
		m_pEditorCtrls->m_treeCtrl.Select(hItem, TVGN_CARET);
		CString itemText = m_pEditorCtrls->m_treeCtrl.GetItemText(hItem);
		CMenu menu;
		menu.LoadMenu(IDR_MENU2);
		CMenu* subMenu = menu.GetSubMenu(0);

		if (NULL == m_pEditorCtrls->m_treeCtrl.GetParentItem(hItem))
		{
			subMenu->RemoveMenu(0, MF_BYPOSITION);
		}
		else if (itemText == _T("圆形") || itemText == _T("顶点"))
		{
			subMenu->RemoveMenu(0, MF_BYPOSITION);
		}
		else if (itemText == _T("圆形轮廓") || itemText == _T("多边形轮廓"))
		{
			subMenu->RemoveMenu(1, MF_BYPOSITION);
		}

		subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, m_pEditorCtrls->m_pMainDlg);
	}
	else if ((m_pEditorCtrls->m_curSelItemType == EICON_CIRCLE || m_pEditorCtrls->m_curSelItemType == EICON_POLYGON) && rc.PtInRect(pt))
	{
		CMenu menu;
		menu.LoadMenu(IDR_MENU3);
		CMenu* subMenu = menu.GetSubMenu(0);

		int nListItem = m_pEditorCtrls->m_listCtrl.HitTest(pt, &uFlags);
		if (nListItem == -1)
		{
			subMenu->RemoveMenu(1, MF_BYPOSITION);
		}

		m_pEditorCtrls->m_listCtrl.ClientToScreen(&pt);
		subMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_pEditorCtrls->m_pMainDlg);
	}
}

LRESULT CMsgModelVisitor::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pGridPropCtrl = (CMFCPropertyGridProperty*)lParam;
	CString parentPropName = pGridPropCtrl->GetParent() ? pGridPropCtrl->GetParent()->GetName() : _T("");
	CString propName = pGridPropCtrl->GetName();
	CString propValue = pGridPropCtrl->GetValue();
	if (parentPropName == _T("属性"))
	{
		if (propName == _T("模型名称"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			if (pGridData && propValue != pGridData->m_prop.m_modelName)
			{
				pGridData->m_prop.m_modelName = propValue;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("材质名称"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			if (pGridData && propValue != pGridData->m_prop.m_materialName)
			{
				pGridData->m_prop.m_materialName = propValue;
				pGridData->m_needUpdateModel = true;
			}
		}
	}
	else if (parentPropName == _T("位置"))
	{
		if (propName == _T("基准平面"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			if (pGridData && propValue != pGridData->m_place.m_plane)
			{
				pGridData->m_place.m_plane = propValue;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("X轴平移"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._translation._x))
			{
				pGridData->m_place.m_spaceInfo._translation._x = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Y轴平移"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._translation._y))
			{
				pGridData->m_place.m_spaceInfo._translation._y = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Z轴平移"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._translation._z))
			{
				pGridData->m_place.m_spaceInfo._translation._z = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("X轴旋转"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._rotation._x))
			{
				pGridData->m_place.m_spaceInfo._rotation._x = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Y轴旋转"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._rotation._y))
			{
				pGridData->m_place.m_spaceInfo._rotation._y = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Z轴旋转"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._rotation._z))
			{
				pGridData->m_place.m_spaceInfo._rotation._z = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("X轴缩放"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._scale._x))
			{
				pGridData->m_place.m_spaceInfo._scale._x = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Y轴缩放"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._scale._y))
			{
				pGridData->m_place.m_spaceInfo._scale._y = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
		else if (propName == _T("Z轴缩放"))
		{
			CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridData && !equals(fVal, pGridData->m_place.m_spaceInfo._scale._z))
			{
				pGridData->m_place.m_spaceInfo._scale._z = fVal;
				pGridData->m_needUpdateModel = true;
			}
		}
	}
	else if (parentPropName == _T("圆形"))
	{
		if (propName == _T("直径"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridCir && !equals(fVal, pGridCir->m_diameter))
			{
				pGridCir->m_diameter = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("圆心X"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridCir && !equals(fVal, pGridCir->m_pos._x))
			{
				pGridCir->m_pos._x = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("圆心Y"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridCir && !equals(fVal, pGridCir->m_pos._y))
			{
				pGridCir->m_pos._y = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("采样点"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			unsigned int uVal = (unsigned int)_wtoi(propValue.GetString());
			if (pGridCir && uVal != pGridCir->m_cnt)
			{
				pGridCir->m_cnt = uVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("顺时针"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridCircle* pGridCir = (CGridCircle*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			bool bVal = _wtoi(propValue.GetString()) != 0;
			if (pGridCir && bVal != pGridCir->m_clockwise)
			{
				pGridCir->m_clockwise = bVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
	}
	else if (parentPropName == _T("顶点"))
	{
		if (propName == _T("点坐标X"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridPolyPt && !equals(fVal, pGridPolyPt->m_pos._x))
			{
				pGridPolyPt->m_pos._x = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("点坐标Y"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridPolyPt && !equals(fVal, pGridPolyPt->m_pos._y))
			{
				pGridPolyPt->m_pos._y = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("角度"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			float fVal = (float)_wtof(propValue.GetString());
			if (pGridPolyPt && !equals(fVal, pGridPolyPt->m_degree))
			{
				pGridPolyPt->m_degree = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("采样点"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			unsigned int uVal = (unsigned int)_wtoi(propValue.GetString());
			if (pGridPolyPt && uVal != pGridPolyPt->m_cnt)
			{
				pGridPolyPt->m_cnt = uVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("顺时针"))
		{
			HTREEITEM hItem = m_pEditorCtrls->m_treeCtrl.GetSelectedItem();
			CGridPolygon::SPolyPoint* pGridPolyPt = (CGridPolygon::SPolyPoint*)m_pEditorCtrls->m_treeCtrl.GetItemData(hItem);
			bool bVal = _wtoi(propValue.GetString()) != 0;
			if (pGridPolyPt && bVal != pGridPolyPt->m_clockwise)
			{
				pGridPolyPt->m_clockwise = bVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
	}
	else if (parentPropName == _T("拉伸"))
	{
		if (propName == _T("拉伸侧面材质"))
		{
			CGridCircle* pGridCircle;
			CGridPolygon* pGridPoly;
			switch (m_pEditorCtrls->m_curSelItemType)
			{
			case EICON_CIRCLE:
				if (m_pEditorCtrls->m_curSelTreeCtrlItem != NULL)
				{
					pGridCircle = (CGridCircle*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_curSelTreeCtrlItem);
					if (pGridCircle)
					{
						pGridCircle->m_shapeOp.m_stretchingBodies.materialName = propValue;
						if (m_pEditorCtrls->m_hParentModelItem)
						{
							CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
							if (pGridData)
								pGridData->m_needUpdateModel = true;
						}
					}
				}
				break;
			case EICON_POLYGON:
				if (m_pEditorCtrls->m_curSelTreeCtrlItem != NULL)
				{
					pGridPoly = (CGridPolygon*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_curSelTreeCtrlItem);
					if (pGridPoly)
					{
						pGridPoly->m_shapeOp.m_stretchingBodies.materialName = propValue;
						if (m_pEditorCtrls->m_hParentModelItem)
						{
							CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
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
		else if (propName == _T("顺时针"))
		{
			CGridCircle* pGridCircle;
			CGridPolygon* pGridPoly;
			switch (m_pEditorCtrls->m_curSelItemType)
			{
			case EICON_CIRCLE:
				if (m_pEditorCtrls->m_curSelTreeCtrlItem != NULL)
				{
					pGridCircle = (CGridCircle*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_curSelTreeCtrlItem);
					if (pGridCircle)
					{
						pGridCircle->m_shapeOp.m_stretchingBodies.reverse = _wtoi(propValue.GetString()) != 0;
						if (m_pEditorCtrls->m_hParentModelItem)
						{
							CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
							if (pGridData)
								pGridData->m_needUpdateModel = true;
						}
					}
				}
				break;
			case EICON_POLYGON:
				if (m_pEditorCtrls->m_curSelTreeCtrlItem != NULL)
				{
					pGridPoly = (CGridPolygon*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_curSelTreeCtrlItem);
					if (pGridPoly)
					{
						pGridPoly->m_shapeOp.m_stretchingBodies.reverse = _wtoi(propValue.GetString()) != 0;
						if (m_pEditorCtrls->m_hParentModelItem)
						{
							CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
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
		else if (propName == _T("X轴平移"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_pEditorCtrls->m_listCtrl.GetItemData(m_pEditorCtrls->m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_translation._x))
			{
				pSpaceData->_translation._x = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Y轴平移"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_pEditorCtrls->m_listCtrl.GetItemData(m_pEditorCtrls->m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_translation._y))
			{
				pSpaceData->_translation._y = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Z轴平移"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_pEditorCtrls->m_listCtrl.GetItemData(m_pEditorCtrls->m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_translation._z))
			{
				pSpaceData->_translation._z = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("X轴旋转"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_pEditorCtrls->m_listCtrl.GetItemData(m_pEditorCtrls->m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_rotation._x))
			{
				pSpaceData->_rotation._x = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Y轴旋转"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_pEditorCtrls->m_listCtrl.GetItemData(m_pEditorCtrls->m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_rotation._y))
			{
				pSpaceData->_rotation._y = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Z轴旋转"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_pEditorCtrls->m_listCtrl.GetItemData(m_pEditorCtrls->m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_rotation._z))
			{
				pSpaceData->_rotation._z = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("X轴缩放"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_pEditorCtrls->m_listCtrl.GetItemData(m_pEditorCtrls->m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_scale._x))
			{
				pSpaceData->_scale._x = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Y轴缩放"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_pEditorCtrls->m_listCtrl.GetItemData(m_pEditorCtrls->m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_scale._y))
			{
				pSpaceData->_scale._y = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
		else if (propName == _T("Z轴缩放"))
		{
			SpatialInfo* pSpaceData = (SpatialInfo*)m_pEditorCtrls->m_listCtrl.GetItemData(m_pEditorCtrls->m_listCtrl.GetSelectionMark());
			float fVal = (float)_wtof(propValue.GetString());
			if (pSpaceData && !equals(fVal, pSpaceData->_scale._z))
			{
				pSpaceData->_scale._z = fVal;
				if (m_pEditorCtrls->m_hParentModelItem)
				{
					CGridData* pGridData = (CGridData*)m_pEditorCtrls->m_treeCtrl.GetItemData(m_pEditorCtrls->m_hParentModelItem);
					if (pGridData)
						pGridData->m_needUpdateModel = true;
				}
			}
		}
	}
	else if (parentPropName == _T("盖顶"))
	{
		if (!propName.IsEmpty())
			m_pEditorCtrls->TopCapPropertyChanged(propName, propValue);
	}
	else if (parentPropName == _T("盖底"))
	{
		if (!propName.IsEmpty())
			m_pEditorCtrls->BottomCapPropertyChanged(propName, propValue);
	}
	return 0;
}
