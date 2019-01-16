#pragma once
#include "MsgVisitor.h"

class CModelEditorCtrls;
class CTessgridBuilderDlg;
class CMsgModelVisitor : public CMsgVisitor
{
public:
	CMsgModelVisitor(CTessgridBuilderDlg* dlg);
	~CMsgModelVisitor();
	void OnSize(CRect clientRc, CRect tabctrlRc);
	void ShowCtrls(bool bShow);
	void OnNewBuild();
	void OnOpenFile();
	void OnSaveFile();
	void OnSaveAs();
	void OnExpandTreeCtrl();
	void OnContextMenu(CWnd* pWnd, CPoint point);
	LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);

private:
	CModelEditorCtrls* m_pEditorCtrls;
};
