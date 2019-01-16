#pragma once

class CTessgridBuilderDlg;
class CBgCustomCtrl : public CStatic
{
	DECLARE_MESSAGE_MAP()
	CTessgridBuilderDlg* m_pMainDlg;
public:
	CBgCustomCtrl();
	void Init(CTessgridBuilderDlg* pDlg);

protected:
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
};
