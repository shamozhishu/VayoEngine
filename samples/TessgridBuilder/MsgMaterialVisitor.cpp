#include "stdafx.h"
#include "MsgMaterialVisitor.h"
#include "TessgridBuilderDlg.h"
#include "MaterialDataManager.h"

CMsgMaterialVisitor::CMsgMaterialVisitor(CTessgridBuilderDlg* dlg)
	: m_pEditorCtrls(&(dlg->m_materialEditorCtrls))
{
}

CMsgMaterialVisitor::~CMsgMaterialVisitor()
{
}

void CMsgMaterialVisitor::OnSize(CRect clientRc, CRect tabctrlRc)
{
	m_pEditorCtrls->m_listCtrl.MoveWindow(tabctrlRc);
	CRect rc = CRect(clientRc.Width() - 260, 42, clientRc.Width(), clientRc.Height() - 20);
	m_pEditorCtrls->m_staPropGridBg.MoveWindow(rc);
	m_pEditorCtrls->m_propGridCtrl.MoveWindow(CRect(0, 0, rc.Width() - 1, rc.Height() - 1));
}

void CMsgMaterialVisitor::ShowCtrls(bool bShow)
{
	int nCmdShow = (bShow ? SW_SHOW : SW_HIDE);
	m_pEditorCtrls->m_listCtrl.ShowWindow(nCmdShow);
	m_pEditorCtrls->m_staPropGridBg.ShowWindow(nCmdShow);
	m_pEditorCtrls->m_propGridCtrl.ShowWindow(nCmdShow);
}

void CMsgMaterialVisitor::OnNewBuild()
{

}

void CMsgMaterialVisitor::OnOpenFile()
{
	CString strTmp = ConfigManager::getSingleton().getConfig()._3d.materialsPath.c_str();
	CFileDialog dlg(TRUE, strTmp, _T(""), OFN_HIDEREADONLY | OFN_READONLY, _T("文件 (*.material)|*.material||"), NULL);
	if (IDOK == dlg.DoModal())
	{
		strTmp = dlg.GetPathName();
		if (CMaterialDataManager::GetIns().OpenMaterialFile(strTmp))
		{
			m_pEditorCtrls->m_savePath = strTmp;
			//CMaterialDataManager::GetIns().ClearAllMaterial();
		}
		else
		{
			strTmp.Format(_T("打开文件[%s]失败！"), strTmp);
			AfxMessageBox(strTmp);
		}
	}
}

void CMsgMaterialVisitor::OnSaveFile()
{

}

void CMsgMaterialVisitor::OnSaveAs()
{

}

void CMsgMaterialVisitor::OnExpandTreeCtrl()
{

}

void CMsgMaterialVisitor::OnContextMenu(CWnd* pWnd, CPoint point)
{

}

LRESULT CMsgMaterialVisitor::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	return 0;
}
