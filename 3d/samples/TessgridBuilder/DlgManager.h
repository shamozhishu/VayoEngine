#pragma once

class CTessgridBuilderDlg;
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
	PROPERTY_R(CTessgridBuilderDlg*, m_pTessgridBuilderDlg, TessgridBuilderDlg)
};
