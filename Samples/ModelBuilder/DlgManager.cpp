#include "stdafx.h"
#include "DlgManager.h"
#include "ModelBuilderDlg.h"

CDlgManager* CDlgManager::ms_dlgMgr = NULL;
CDlgManager* CDlgManager::GetIns()
{
	if (!ms_dlgMgr)
	{
		ms_dlgMgr = new CDlgManager();
		ms_dlgMgr->Init();
	}
	return ms_dlgMgr;
}

void CDlgManager::Destroy()
{
	delete ms_dlgMgr;
	ms_dlgMgr = NULL;
}

CDlgManager::CDlgManager()
	: m_pModelBuilderDlg(NULL)
{
}

CDlgManager::~CDlgManager()
{
	CModelBuilderDlg::FinishDlg(m_pModelBuilderDlg);
}

void CDlgManager::Init()
{
	CModelBuilderDlg::FinishDlg(m_pModelBuilderDlg);
	m_pModelBuilderDlg = CModelBuilderDlg::StartDlg(NULL);
}

void CDlgManager::AutoWidthList(CListCtrl& opList)
{
	int nColumnCount = opList.GetHeaderCtrl()->GetItemCount();
	for (int i = 0; i < nColumnCount; i++)
	{
		opList.SetColumnWidth(i, LVSCW_AUTOSIZE);
		int nColumnWidth = opList.GetColumnWidth(i);
		opList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		int nHeaderWidth = opList.GetColumnWidth(i);
		opList.SetColumnWidth(i, max(nColumnWidth, nHeaderWidth) + 2);
	}
}
