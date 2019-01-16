#include "stdafx.h"
#include "BgCustomCtrl.h"
#include "TessgridBuilderDlg.h"
#include "DlgManager.h"

BEGIN_MESSAGE_MAP(CBgCustomCtrl, CStatic)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &OnPropertyChanged)
END_MESSAGE_MAP()

CBgCustomCtrl::CBgCustomCtrl()
	: m_pMainDlg(NULL)
{
}

void CBgCustomCtrl::Init(CTessgridBuilderDlg* pDlg)
{
	m_pMainDlg = pDlg;
}

LRESULT CBgCustomCtrl::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	if (m_pMainDlg)
		return m_pMainDlg->OnPropertyChanged(wParam, lParam);
	return 0;
}
