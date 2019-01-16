#pragma once
#include "MsgVisitor.h"

class CTessgridBuilderDlg;
class CMaterialEditorCtrls;
class CMsgMaterialVisitor : public CMsgVisitor
{
public:
	CMsgMaterialVisitor(CTessgridBuilderDlg* dlg);
	~CMsgMaterialVisitor();
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
	CMaterialEditorCtrls* m_pEditorCtrls;
};
