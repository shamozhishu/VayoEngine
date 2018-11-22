#pragma once

class CModelBuilderDlg;
class CDlgManager
{
public:
	static CDlgManager* GetIns();
	static void Destroy();
	CDlgManager();
	~CDlgManager();
	void Init();
	void AutoWidthList(CListCtrl& opList);

private:
	static CDlgManager* ms_dlgMgr;
private:
	PROPERTY_R(CModelBuilderDlg*, m_pModelBuilderDlg, ModelBuilderDlg)
};
