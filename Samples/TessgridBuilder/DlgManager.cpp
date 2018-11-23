#include "stdafx.h"
#include "DlgManager.h"
#include "TessgridBuilderDlg.h"

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
	: m_pTessgridBuilderDlg(NULL)
{
}

CDlgManager::~CDlgManager()
{
	CTessgridBuilderDlg::FinishDlg(m_pTessgridBuilderDlg);
}

void CDlgManager::Init()
{
	CTessgridBuilderDlg::FinishDlg(m_pTessgridBuilderDlg);
	m_pTessgridBuilderDlg = CTessgridBuilderDlg::StartDlg(NULL);
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
